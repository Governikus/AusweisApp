/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
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

void RemoteServerImpl::onConnectedChanged(bool pConnected)
{
	if (pConnected)
	{
		mRemoteReaderAdvertiser.reset();
		return;
	}

	if (isRunning() && !pConnected)
	{
		const auto& ifdName = mWebSocketServer->getServerName();
		const auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		const auto& ifdId = RemoteServiceSettings::generateFingerprint(remoteServiceSettings.getCertificate());
		quint16 port = mWebSocketServer->getServerPort();
		mRemoteReaderAdvertiser.reset(Env::create<RemoteReaderAdvertiser*>(ifdName, ifdId, port));
		return;
	}
}


RemoteServerImpl::RemoteServerImpl()
	: RemoteServer()
	, mRemoteReaderAdvertiser()
	, mWebSocketServer(Env::create<RemoteWebSocketServer*>())
{
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::fireConnectedChanged, this, &RemoteServerImpl::onConnectedChanged);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::fireConnectedChanged, this, &RemoteServerImpl::fireConnectedChanged);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::fireMessageHandlerAdded, this, &RemoteServer::fireMessageHandlerAdded);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::firePskChanged, this, &RemoteServer::firePskChanged);
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
	return success;
}


void RemoteServerImpl::stop()
{
	mRemoteReaderAdvertiser.reset();
	mWebSocketServer->close();
}


void RemoteServerImpl::setPairing(bool pEnable)
{
	mWebSocketServer->setPairing(pEnable);
}


bool RemoteServerImpl::isConnected() const
{
	return mWebSocketServer->isConnected();
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
