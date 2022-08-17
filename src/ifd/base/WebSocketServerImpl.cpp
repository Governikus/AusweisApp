/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "WebSocketServerImpl.h"

#include "AppSettings.h"
#include "Env.h"
#include "WebSocketChannel.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ifd)

using namespace governikus;

void WebSocketServerImpl::onWebsocketConnection()
{
	if (!mServer.hasPendingConnections())
	{
		return;
	}

	QSharedPointer<QWebSocket> connection(mServer.nextPendingConnection());
	if (mServerMessageHandler)
	{
		connection->close(QWebSocketProtocol::CloseCodePolicyViolated);
		return;
	}

	QSharedPointer<DataChannel> channel(new WebSocketChannel(connection), &QObject::deleteLater);
	mServerMessageHandler.reset(Env::create<ServerMessageHandler*>(channel, mAllowedPlugInTypes));
	connect(mServerMessageHandler.data(), &ServerMessageHandler::fireClosed, this, &WebSocketServerImpl::onConnectionClosed);
	Q_EMIT fireMessageHandlerAdded(mServerMessageHandler);
	Q_EMIT fireNewConnection(connection);
	Q_EMIT fireConnectedChanged(isConnected());
}


void WebSocketServerImpl::onConnectionClosed()
{
	mServerMessageHandler.reset();
	Q_EMIT fireConnectedChanged(isConnected());
}


void WebSocketServerImpl::onServerError(QWebSocketProtocol::CloseCode pCloseCode) const
{
	qCCritical(ifd) << pCloseCode;
}


WebSocketServerImpl::WebSocketServerImpl(QSharedPointer<TlsServer> pTlsServer, const QVector<ReaderManagerPlugInType>& pAllowedPlugInTypes)
	: WebSocketServer()
	, mServer(QString(), QWebSocketServer::NonSecureMode)
	, mTlsServer(pTlsServer)
	, mServerMessageHandler()
	, mAllowedPlugInTypes(pAllowedPlugInTypes)
{
	Q_ASSERT(mTlsServer);

	connect(mTlsServer.data(), &TlsServer::fireNewConnection, &mServer, &QWebSocketServer::handleConnection);
	connect(mTlsServer.data(), &TlsServer::firePskChanged, this, &WebSocketServerImpl::firePskChanged);
	connect(&mServer, &QWebSocketServer::newConnection, this, &WebSocketServerImpl::onWebsocketConnection);
}


WebSocketServerImpl::~WebSocketServerImpl()
{
	Q_ASSERT(mTlsServer);

	if (mTlsServer->isListening())
	{
		qCDebug(ifd) << "Shutdown tls server";
		mTlsServer->close();
	}
}


bool WebSocketServerImpl::isListening() const
{
	Q_ASSERT(mTlsServer);

	return mTlsServer->isListening();
}


bool WebSocketServerImpl::isConnected() const
{
	return !mServerMessageHandler.isNull();
}


void WebSocketServerImpl::close()
{
	Q_ASSERT(mTlsServer);

	mTlsServer->close();
	mServerMessageHandler.reset();
}


bool WebSocketServerImpl::listen(const QString& pServerName, quint16 pPort)
{
	Q_ASSERT(mTlsServer);

	mServer.setServerName(pServerName);
	return mTlsServer->startListening(pPort);
}


QString WebSocketServerImpl::getServerName() const
{
	return isListening() ? mServer.serverName() : QString();
}


QHostAddress WebSocketServerImpl::getServerAddress() const
{
	Q_ASSERT(mTlsServer);

	return mTlsServer->serverAddress();
}


quint16 WebSocketServerImpl::getServerPort() const
{
	Q_ASSERT(mTlsServer);

	return isListening() ? mTlsServer->serverPort() : 0;
}


const QSharedPointer<ServerMessageHandler>& WebSocketServerImpl::getMessageHandler() const
{
	return mServerMessageHandler;
}
