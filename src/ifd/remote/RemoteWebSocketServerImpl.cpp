/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteWebSocketServerImpl.h"

#include "Env.h"
#include "RemoteTlsServer.h"
#include "SecureStorage.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)

namespace governikus
{
template<> RemoteWebSocketServer* createNewObject<RemoteWebSocketServer*>()
{
	return new RemoteWebSocketServerImpl();
}


} // namespace governikus

using namespace governikus;


void RemoteWebSocketServerImpl::onNewConnection(QSharedPointer<QWebSocket> pSocket)
{
	const auto& cfg = pSocket->sslConfiguration();
	const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getCiphers();
	mPairingConnection = pairingCiphers.contains(cfg.sessionCipher());
	Q_EMIT fireNewConnection(pSocket);
}


RemoteWebSocketServerImpl::RemoteWebSocketServerImpl()
	: RemoteWebSocketServer()
	, mRemoteTlsServer(QSharedPointer<RemoteTlsServer>::create())
	, mWebSocketServer(mRemoteTlsServer, {ReaderManagerPlugInType::NFC})
	, mPairingConnection(false)
{
	connect(mRemoteTlsServer.data(), &RemoteTlsServer::firePairingCompleted, this, &RemoteWebSocketServer::firePairingCompleted);
	connect(&mWebSocketServer, &WebSocketServer::fireNewConnection, this, &RemoteWebSocketServerImpl::onNewConnection);
	connect(&mWebSocketServer, &WebSocketServer::fireConnectedChanged, this, &RemoteWebSocketServerImpl::fireConnectedChanged);
	connect(&mWebSocketServer, &WebSocketServer::fireMessageHandlerAdded, this, &RemoteWebSocketServerImpl::fireMessageHandlerAdded);
	connect(&mWebSocketServer, &WebSocketServer::firePskChanged, this, &RemoteWebSocketServerImpl::firePskChanged);
}


bool RemoteWebSocketServerImpl::isListening() const
{
	return mWebSocketServer.isListening();
}


bool RemoteWebSocketServerImpl::isConnected() const
{
	return mWebSocketServer.isConnected();
}


bool RemoteWebSocketServerImpl::listen(const QString& pServerName, quint16 pPort)
{
	return mWebSocketServer.listen(pServerName, pPort);
}


void RemoteWebSocketServerImpl::close()
{
	mWebSocketServer.close();
}


QString RemoteWebSocketServerImpl::getServerName() const
{
	return mWebSocketServer.getServerName();
}


QHostAddress RemoteWebSocketServerImpl::getServerAddress() const
{
	return mWebSocketServer.getServerAddress();
}


quint16 RemoteWebSocketServerImpl::getServerPort() const
{
	return mWebSocketServer.getServerPort();
}


const QSharedPointer<ServerMessageHandler>& RemoteWebSocketServerImpl::getMessageHandler() const
{
	return mWebSocketServer.getMessageHandler();
}


bool RemoteWebSocketServerImpl::isPairingConnection() const
{
	return mPairingConnection;
}


void RemoteWebSocketServerImpl::setPairing(bool pEnable)
{
	mRemoteTlsServer->setPairing(pEnable);
}


QSslCertificate RemoteWebSocketServerImpl::getCurrentCertificate() const
{
	return mRemoteTlsServer->getCurrentCertificate();
}
