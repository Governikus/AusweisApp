/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginWebSocket.h"

#include "Env.h"
#include "ReaderManager.h"
#include "UiLoader.h"
#include "VolatileSettings.h"
#include "WorkflowRequest.h"

#include <QCoreApplication>
#include <QFile>
#include <QHostAddress>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QRegularExpression>
#include <QSignalBlocker>


Q_DECLARE_LOGGING_CATEGORY(websocket)

using namespace governikus;


UiPluginWebSocket::UiPluginWebSocket()
	: UiPlugin()
	, mServer(QCoreApplication::applicationName() + QLatin1Char('/') + QCoreApplication::applicationVersion(), QWebSocketServer::NonSecureMode)
	, mConnection(nullptr)
	, mRequest()
	, mJson(nullptr)
	, mContext()
	, mUiDomination(false)
	, mUiDominationPrevUsedAsSDK(false)
{
}


bool UiPluginWebSocket::initialize()
{
	if (mHttpServer)
	{
		return true;
	}

	if (!Env::getSingleton<UiLoader>()->load<UiPluginJson>())
	{
		qCWarning(websocket) << "Cannot start WebSocket because JSON-API is missing";
		return false;
	}

	mJson = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
	mHttpServer = Env::getShared<HttpServer>();
	if (!mHttpServer->isListening())
	{
		mHttpServer.clear();
		return false;
	}

	qCDebug(websocket) << "Enable WebSocket...";
	connect(mHttpServer.data(), &HttpServer::fireNewWebSocketRequest, this, &UiPluginWebSocket::onNewWebSocketRequest);
	connect(mHttpServer.data(), &HttpServer::fireRebound, this, [this]{
			if (!mHttpServer->isListening())
			{
				qCDebug(websocket) << "Disable WebSocket...";
				disconnect(mHttpServer.data(), &HttpServer::fireNewWebSocketRequest, this, &UiPluginWebSocket::onNewWebSocketRequest);
				mHttpServer.clear();
			}
		});
	connect(&mServer, &QWebSocketServer::newConnection, this, &UiPluginWebSocket::onNewConnection);
	return true;
}


void UiPluginWebSocket::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (mUiDomination)
	{
		mContext = pRequest->getContext();
		mContext->claim(this);
		mContext->setReaderPluginTypes({ReaderManagerPluginType::PCSC, ReaderManagerPluginType::REMOTE_IFD, ReaderManagerPluginType::SIMULATOR});
	}
}


void UiPluginWebSocket::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)

	mContext.clear();
}


void UiPluginWebSocket::onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted)
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


void UiPluginWebSocket::onUiDominationReleased()
{
	if (mUiDomination)
	{
		mUiDomination = false;
		mJson->setEnabled(false);
		Env::getSingleton<ReaderManager>()->stopScanAll();
		Env::getSingleton<VolatileSettings>()->setUsedAsSDK(mUiDominationPrevUsedAsSDK);
	}
}


void UiPluginWebSocket::onNewWebSocketRequest(const QSharedPointer<HttpRequest>& pRequest)
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


void UiPluginWebSocket::onNewConnection()
{
	if (mServer.hasPendingConnections())
	{
		mConnection.reset(mServer.nextPendingConnection());
		connect(mJson, &UiPluginJson::fireMessage, this, &UiPluginWebSocket::onJsonMessage);
		connect(mConnection.data(), &QWebSocket::textMessageReceived, this, &UiPluginWebSocket::onTextMessageReceived);
		connect(mConnection.data(), &QWebSocket::disconnected, this, &UiPluginWebSocket::onClientDisconnected);
		mJson->setEnabled();
	}
	else
	{
		mConnection.reset();
		mRequest.reset();
		Q_EMIT fireUiDominationRelease();
	}
}


void UiPluginWebSocket::onClientDisconnected()
{
	qCDebug(websocket) << "Client disconnected...";

	if (mContext && mUiDomination)
	{
		const QSignalBlocker blocker(mJson);
		mContext->killWorkflow();
	}

	mConnection.reset();
	mRequest.reset();
	disconnect(mJson, &UiPluginJson::fireMessage, this, &UiPluginWebSocket::onJsonMessage);
	Q_EMIT fireUiDominationRelease();
}


void UiPluginWebSocket::onTextMessageReceived(const QString& pMessage)
{
	if (mConnection)
	{
		mJson->doMessageProcessing(pMessage.toUtf8());
	}
}


void UiPluginWebSocket::onJsonMessage(const QByteArray& pMessage)
{
	if (mConnection)
	{
		mConnection->sendTextMessage(QString::fromUtf8(pMessage));
	}
}


void UiPluginWebSocket::doShutdown()
{
	if (mConnection)
	{
		mConnection->close(QWebSocketProtocol::CloseCodeGoingAway);
		mConnection->disconnect(this);
		mConnection.reset();
	}

	if (mHttpServer)
	{
		mHttpServer->disconnect(this);
		mHttpServer.reset();
	}
}
