/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteIfdServer.h"

#include "AppSettings.h"
#include "Env.h"
#include "TlsChecker.h"

using namespace governikus;


void RemoteIfdServer::onPskChanged(const QByteArray& pPsk)
{
	if (mRemoteReaderAdvertiser)
	{
		mRemoteReaderAdvertiser->setPairing(!pPsk.isEmpty());
	}
	Q_EMIT firePskChanged(pPsk);
}


void RemoteIfdServer::onConnectedChanged(bool pConnected)
{
	if (pConnected)
	{
		mRemoteReaderAdvertiser.reset();
	}
	else if (isRunning())
	{
		const auto& ifdName = mWebSocketServer->getServerName();
		const auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		const auto& ifdId = QString::fromLatin1(TlsChecker::getRootCertificate(remoteServiceSettings.getCertificates()).toPem());
		quint16 port = mWebSocketServer->getServerPort();
		bool isPairing = mWebSocketServer->isPairingAnnounced();
		mRemoteReaderAdvertiser.reset(Env::create<RemoteReaderAdvertiser*>(ifdName, ifdId, port, isPairing));
	}

	Q_EMIT fireConnectedChanged(pConnected);
}


RemoteIfdServer::RemoteIfdServer()
	: IfdServer()
	, mRemoteReaderAdvertiser()
	, mWebSocketServer(Env::create<RemoteWebSocketServer*>())
{
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::firePskChanged, this, &RemoteIfdServer::onPskChanged);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::fireConnectedChanged, this, &RemoteIfdServer::onConnectedChanged);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::fireMessageHandlerAdded, this, &IfdServer::fireMessageHandlerAdded);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::firePairingCompleted, this, &IfdServer::firePairingCompleted);
	connect(mWebSocketServer.data(), &RemoteWebSocketServer::fireSocketError, this, &IfdServer::fireSocketError);
	connect(mWebSocketServer.data(), &WebSocketServer::fireNameChanged, this, &IfdServer::fireNameChanged);
}


bool RemoteIfdServer::isRunning() const
{
	return mWebSocketServer->isListening();
}


bool RemoteIfdServer::start(const QString& pServerName)
{
	bool success = mWebSocketServer->listen(pServerName);
	if (success)
	{
		onConnectedChanged(false);
	}
	Q_EMIT fireIsRunningChanged();
	return success;
}


void RemoteIfdServer::stop()
{
	mRemoteReaderAdvertiser.reset();
	mWebSocketServer->close();
	Q_EMIT fireIsRunningChanged();
}


void RemoteIfdServer::setPairing(bool pEnable)
{
	mWebSocketServer->setPairing(pEnable);
}


bool RemoteIfdServer::isConnected() const
{
	return mWebSocketServer->isConnected();
}


bool RemoteIfdServer::isPairingConnection() const
{
	return mWebSocketServer->isPairingConnection();
}


QSslCertificate RemoteIfdServer::getCurrentCertificate() const
{
	return mWebSocketServer->getCurrentCertificate();
}


const QSharedPointer<ServerMessageHandler>& RemoteIfdServer::getMessageHandler() const
{
	Q_ASSERT(mWebSocketServer);
	return mWebSocketServer->getMessageHandler();
}


bool RemoteIfdServer::isLocal() const
{
	return false;
}
