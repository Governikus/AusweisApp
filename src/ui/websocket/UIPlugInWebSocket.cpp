/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInWebSocket.h"

#include "AppSettings.h"
#include "Env.h"
#include "ReaderManager.h"
#include "UILoader.h"

#include <QCoreApplication>
#include <QFile>
#include <QHostAddress>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QRegularExpression>


Q_DECLARE_LOGGING_CATEGORY(websocket)

using namespace governikus;


UIPlugInWebSocket::UIPlugInWebSocket()
	: UIPlugIn()
	, mServer(QCoreApplication::applicationName() + QLatin1Char('/') + QCoreApplication::applicationVersion(), QWebSocketServer::NonSecureMode)
	, mConnection(nullptr)
	, mRequest()
	, mJsonApi(nullptr)
	, mContext()
	, mUiDomination(false)
{
	if (!UILoader::getInstance().load(UIPlugInName::UIPlugInJsonApi))
	{
		qCWarning(websocket) << "Cannot start WebSocket because JSON-API is missing";
		return;
	}

	mJsonApi = qobject_cast<UIPlugInJsonApi*>(UILoader::getInstance().getLoaded(UIPlugInName::UIPlugInJsonApi));
	Q_ASSERT(mJsonApi);

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
	Q_UNUSED(pContext);

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
		Env::getSingleton<AppSettings>()->setUsedAsSDK(true);
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
	mUiDomination = false;
	mJsonApi->setEnabled(false);
	Env::getSingleton<ReaderManager>()->stopScanAll();
	Env::getSingleton<AppSettings>()->setUsedAsSDK(false);
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
		connect(mJsonApi, &UIPlugInJsonApi::fireMessage, this, &UIPlugInWebSocket::onJsonApiMessage);
		connect(mConnection.data(), &QWebSocket::textMessageReceived, this, &UIPlugInWebSocket::onTextMessageReceived);
		connect(mConnection.data(), &QWebSocket::disconnected, this, &UIPlugInWebSocket::onClientDisconnected);
		mJsonApi->setEnabled();
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
		const QSignalBlocker blocker(mJsonApi);
		Q_EMIT mContext->fireCancelWorkflow();
	}

	mConnection.reset();
	mRequest.reset();
	disconnect(mJsonApi, &UIPlugInJsonApi::fireMessage, this, &UIPlugInWebSocket::onJsonApiMessage);
	Q_EMIT fireUiDominationRelease();
}


void UIPlugInWebSocket::onTextMessageReceived(const QString& pMessage)
{
	if (mConnection)
	{
		mJsonApi->doMessageProcessing(pMessage.toUtf8());
	}
}


void UIPlugInWebSocket::onJsonApiMessage(const QByteArray& pMessage)
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
