/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteWebSocketServer.h"

#include "AppSettings.h"
#include "Env.h"
#include "SecureStorage.h"
#include "ServerMessageHandler.h"
#include "WebSocketChannel.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QSharedPointer>
#include <QSslPreSharedKeyAuthenticator>


Q_DECLARE_LOGGING_CATEGORY(remote_device)

namespace governikus
{
template<> RemoteWebSocketServer* createNewObject<RemoteWebSocketServer*>()
{
	return new RemoteWebSocketServerImpl;
}


} /* namespace governikus */

using namespace governikus;


RemoteWebSocketServer::~RemoteWebSocketServer()
{
}


void RemoteWebSocketServerImpl::onWebsocketConnection()
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
	mServerMessageHandler.reset(Env::create<ServerMessageHandler*>(channel));
	connect(mServerMessageHandler.data(), &ServerMessageHandler::fireClosed, this, &RemoteWebSocketServerImpl::onConnectionClosed);
	Q_EMIT fireConnectedChanged(isConnected());
	Q_EMIT fireMessageHandlerAdded(mServerMessageHandler);
}


void RemoteWebSocketServerImpl::onConnectionClosed()
{
	mServerMessageHandler.reset();
	Q_EMIT fireConnectedChanged(isConnected());
}


void RemoteWebSocketServerImpl::onServerError(QWebSocketProtocol::CloseCode pCloseCode)
{
	static int timesLogged = 0;
	if (timesLogged < 20)
	{
		qCCritical(remote_device) << pCloseCode;
		timesLogged++;
	}
}


RemoteWebSocketServerImpl::RemoteWebSocketServerImpl()
	: mTlsServer(new RemoteTlsServer)
	, mServer(QString(), QWebSocketServer::NonSecureMode)
	, mServerMessageHandler()
{
	connect(mTlsServer.data(), &RemoteTlsServer::newConnection, &mServer, &QWebSocketServer::handleConnection);
	connect(mTlsServer.data(), &RemoteTlsServer::firePskChanged, this, &RemoteWebSocketServer::firePskChanged);
	connect(&mServer, &QWebSocketServer::newConnection, this, &RemoteWebSocketServerImpl::onWebsocketConnection);
}


RemoteWebSocketServerImpl::~RemoteWebSocketServerImpl()
{
	if (mTlsServer->isListening())
	{
		qCDebug(remote_device) << "Shutdown tls server";
		mTlsServer->close();
	}
}


bool RemoteWebSocketServerImpl::isListening() const
{
	return mTlsServer->isListening();
}


bool RemoteWebSocketServerImpl::isConnected() const
{
	return !mServerMessageHandler.isNull();
}


bool RemoteWebSocketServerImpl::listen(const QString& pServerName)
{
	mServer.setServerName(pServerName);
	return mTlsServer->listen();
}


void RemoteWebSocketServerImpl::close()
{
	mTlsServer->close();
	mServerMessageHandler.reset();
}


QString RemoteWebSocketServerImpl::getServerName() const
{
	return isListening() ? mServer.serverName() : QString();
}


quint16 RemoteWebSocketServerImpl::getServerPort() const
{
	return isListening() ? mTlsServer->serverPort() : 0;
}


void RemoteWebSocketServerImpl::setPairing(bool pEnable)
{
	mTlsServer->setPairing(pEnable);
}


QSslCertificate RemoteWebSocketServerImpl::getCurrentCertificate() const
{
	return mTlsServer->getCurrentCertificate();
}


const QSharedPointer<ServerMessageHandler>& RemoteWebSocketServerImpl::getMessageHandler() const
{
	return mServerMessageHandler;
}
