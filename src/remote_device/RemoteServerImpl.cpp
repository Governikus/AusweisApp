/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServerImpl.h"

#include "AppSettings.h"
#include "Env.h"

using namespace governikus;


namespace governikus
{

template<> RemoteServer* createNewObject<RemoteServer*>()
{
	return new RemoteServerImpl();
}


} // namespace governikus


void RemoteServerImpl::onPskChanged(const QByteArray& pPsk)
{
	if (mRemoteReaderAdvertiser)
	{
		mRemoteReaderAdvertiser->setPairing(!pPsk.isEmpty());
	}
	Q_EMIT firePskChanged(pPsk);
}


void RemoteServerImpl::onConnectedChanged(bool pConnected)
{
	if (pConnected)
	{
		mRemoteReaderAdvertiser.reset();
	}
	else if (isRunning())
	{
		const auto& ifdName = mWebSocketServer->getServerName();
		const auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		const auto& ifdId = QString::fromLatin1(remoteServiceSettings.getCertificate().toPem());
		quint16 port = mWebSocketServer->getServerPort();
		mRemoteReaderAdvertiser.reset(Env::create<RemoteReaderAdvertiser*>(ifdName, ifdId, port));
	}

	Q_EMIT fireConnectedChanged(pConnected);
}


RemoteServerImpl::RemoteServerImpl()
	: RemoteServer()
	, mRemoteReaderAdvertiser()
	, mWebSocketServer(Env::create<RemoteWebSocketServer*>())
{
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::firePskChanged, this, &RemoteServerImpl::onPskChanged);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::fireConnectedChanged, this, &RemoteServerImpl::onConnectedChanged);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::fireMessageHandlerAdded, this, &RemoteServer::fireMessageHandlerAdded);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::firePairingCompleted, this, &RemoteServer::firePairingCompleted);
}


bool RemoteServerImpl::isRunning() const
{
	return mWebSocketServer->isListening();
}


bool RemoteServerImpl::start(const QString& pServerName)
{
	bool success = mWebSocketServer->listen(pServerName);
	if (success)
	{
		onConnectedChanged(false);
	}
	Q_EMIT fireIsRunningChanged();
	return success;
}


void RemoteServerImpl::stop()
{
	mRemoteReaderAdvertiser.reset();
	mWebSocketServer->close();
	Q_EMIT fireIsRunningChanged();
}


void RemoteServerImpl::setPairing(bool pEnable)
{
	mWebSocketServer->setPairing(pEnable);
}


bool RemoteServerImpl::isConnected() const
{
	return mWebSocketServer->isConnected();
}


bool RemoteServerImpl::isPairingConnection() const
{
	return mWebSocketServer->isPairingConnection();
}


QSslCertificate RemoteServerImpl::getCurrentCertificate() const
{
	return mWebSocketServer->getCurrentCertificate();
}


const QSharedPointer<ServerMessageHandler>& RemoteServerImpl::getMessageHandler() const
{
	Q_ASSERT(mWebSocketServer);
	return mWebSocketServer->getMessageHandler();
}
