/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInWebSocket.h"

#include "Env.h"
#include "ReaderManager.h"
#include "UILoader.h"
#include "VolatileSettings.h"

#include <QCoreApplication>
#include <QFile>
#include <QHostAddress>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QRegularExpression>
#include <QSignalBlocker>


Q_DECLARE_LOGGING_CATEGORY(websocket)

using namespace governikus;


UIPlugInWebSocket::UIPlugInWebSocket()
	: UIPlugIn()
	, mServer(QCoreApplication::applicationName() + QLatin1Char('/') + QCoreApplication::applicationVersion(), QWebSocketServer::NonSecureMode)
	, mConnection(nullptr)
	, mRequest()
	, mJson(nullptr)
	, mContext()
	, mUiDomination(false)
	, mUiDominationPrevUsedAsSDK(false)
{
	if (!Env::getSingleton<UILoader>()->load(UIPlugInName::UIPlugInJson))
	{
		qCWarning(websocket) << "Cannot start WebSocket because JSON-API is missing";
		return;
	}

	mJson = qobject_cast<UIPlugInJson*>(Env::getSingleton<UILoader>()->getLoaded(UIPlugInName::UIPlugInJson));
	Q_ASSERT(mJson);

	mHttpServer = Env::getShared<HttpServer>();
	if (mHttpServer->isListening())
	{
		qCDebug(websocket) << "Enable WebSocket...";
		connect(mHttpServer.data(), &HttpServer::fireNewWebSocketRequest, this, &UIPlugInWebSocket::onNewWebSocketRequest);
		connect(&mServer, &QWebSocketServer::newConnection, this, &UIPlugInWebSocket::onNewConnection);
	}
}


UIPlugInWebSocket::~UIPlugInWebSocket()
{
}


void UIPlugInWebSocket::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	if (mUiDomination)
	{
		pContext->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC, ReaderManagerPlugInType::REMOTE});
		mContext = pContext;
	}
}


void UIPlugInWebSocket::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)

	mContext.clear();
}


void UIPlugInWebSocket::onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted)
{
	Q_UNUSED(pInformation)

	if (pUi != this)
	{
		return;
	}

	if (pAccepted)
	{
		Q_ASSERT(!mConnection);
		mUiDomination = true;
		mUiDominationPrevUsedAsSDK = Env::getSingleton<VolatileSettings>()->isUsedAsSDK();
		Env::getSingleton<VolatileSettings>()->setUsedAsSDK(true);
		Env::getSingleton<ReaderManager>()->startScanAll();
		mServer.handleConnection(mRequest->take());
	}
	else
	{
		mRequest->send(HTTP_STATUS_CONFLICT);
		mRequest.reset();
	}
}


void UIPlugInWebSocket::onUiDominationReleased()
{
	if (mUiDomination)
	{
		mUiDomination = false;
		mJson->setEnabled(false);
		Env::getSingleton<ReaderManager>()->stopScanAll();
		Env::getSingleton<VolatileSettings>()->setUsedAsSDK(mUiDominationPrevUsedAsSDK);
	}
}


void UIPlugInWebSocket::onNewWebSocketRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	const auto& url = pRequest->getUrl();

	const QRegularExpression path(QStringLiteral("^/eID-Kernel(/)?$"));
	if (!path.match(url.path()).hasMatch())
	{
		qCDebug(websocket) << "Request path not supported! Use /eID-Kernel";
		pRequest->send(HTTP_STATUS_NOT_FOUND);
		return;
	}

	if (mRequest)
	{
		qCDebug(websocket) << "Client is already connected...";
		pRequest->send(HTTP_STATUS_TOO_MANY_REQUESTS);
		return;
	}

	mRequest = pRequest;
	Q_EMIT fireUiDominationRequest(this, QString::fromLatin1(pRequest->getHeader().value(QByteArrayLiteral("user-agent"))));
}


void UIPlugInWebSocket::onNewConnection()
{
	if (mServer.hasPendingConnections())
	{
		mConnection.reset(mServer.nextPendingConnection());
		connect(mJson, &UIPlugInJson::fireMessage, this, &UIPlugInWebSocket::onJsonMessage);
		connect(mConnection.data(), &QWebSocket::textMessageReceived, this, &UIPlugInWebSocket::onTextMessageReceived);
		connect(mConnection.data(), &QWebSocket::disconnected, this, &UIPlugInWebSocket::onClientDisconnected);
		mJson->setEnabled();
	}
	else
	{
		mConnection.reset();
		mRequest.reset();
		Q_EMIT fireUiDominationRelease();
	}
}


void UIPlugInWebSocket::onClientDisconnected()
{
	qCDebug(websocket) << "Client disconnected...";

	if (mContext && mUiDomination)
	{
		const QSignalBlocker blocker(mJson);
		mContext->killWorkflow();
	}

	mConnection.reset();
	mRequest.reset();
	disconnect(mJson, &UIPlugInJson::fireMessage, this, &UIPlugInWebSocket::onJsonMessage);
	Q_EMIT fireUiDominationRelease();
}


void UIPlugInWebSocket::onTextMessageReceived(const QString& pMessage)
{
	if (mConnection)
	{
		mJson->doMessageProcessing(pMessage.toUtf8());
	}
}


void UIPlugInWebSocket::onJsonMessage(const QByteArray& pMessage)
{
	if (mConnection)
	{
		mConnection->sendTextMessage(QString::fromUtf8(pMessage));
	}
}


void UIPlugInWebSocket::doShutdown()
{
	if (mConnection)
	{
		mConnection->close(QWebSocketProtocol::CloseCodeGoingAway);
	}

	mHttpServer.reset();
}
