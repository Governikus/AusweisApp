/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "LocalWebSocketServerImpl.h"

#include "Env.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ifd)

using namespace governikus;

namespace governikus
{
template<> LocalWebSocketServer* createNewObject<LocalWebSocketServer*>()
{
	return new LocalWebSocketServerImpl();
}


} // namespace governikus


LocalWebSocketServerImpl::LocalWebSocketServerImpl()
	: LocalWebSocketServer()
	, mLocalTlsServer(QSharedPointer<LocalTlsServer>::create())
	, mWebSocketServer(mLocalTlsServer, {ReaderManagerPluginType::SMART, ReaderManagerPluginType::UNKNOWN})
{
	connect(&mWebSocketServer, &WebSocketServer::fireNewConnection, this, &LocalWebSocketServerImpl::fireNewConnection);
	connect(&mWebSocketServer, &WebSocketServer::fireConnectedChanged, this, &LocalWebSocketServerImpl::fireConnectedChanged);
	connect(&mWebSocketServer, &WebSocketServer::fireMessageHandlerAdded, this, &LocalWebSocketServerImpl::fireMessageHandlerAdded);
	connect(&mWebSocketServer, &WebSocketServer::firePskChanged, this, &LocalWebSocketServerImpl::firePskChanged);
	connect(mLocalTlsServer.data(), &TlsServer::fireSocketError, this, &LocalWebSocketServerImpl::fireSocketError);
}


void LocalWebSocketServerImpl::setPsk(const QByteArray& pPsk)
{
	mLocalTlsServer->setPsk(pPsk);
}


bool LocalWebSocketServerImpl::isListening() const
{
	return mWebSocketServer.isListening();
}


bool LocalWebSocketServerImpl::isConnected() const
{
	return mWebSocketServer.isConnected();
}


bool LocalWebSocketServerImpl::listen(const QString& pServerName, quint16 pPort)
{
	return mWebSocketServer.listen(pServerName, pPort);
}


void LocalWebSocketServerImpl::close()
{
	mWebSocketServer.close();
}


QString LocalWebSocketServerImpl::getServerName() const
{
	return mWebSocketServer.getServerName();
}


QHostAddress LocalWebSocketServerImpl::getServerAddress() const
{
	return mWebSocketServer.getServerAddress();
}


quint16 LocalWebSocketServerImpl::getServerPort() const
{
	return mWebSocketServer.getServerPort();
}


const QSharedPointer<ServerMessageHandler>& LocalWebSocketServerImpl::getMessageHandler() const
{
	return mWebSocketServer.getMessageHandler();
}
