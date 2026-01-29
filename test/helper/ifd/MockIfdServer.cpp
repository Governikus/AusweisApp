/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "MockIfdServer.h"

#include "ServerMessageHandlerImpl.h"


using namespace governikus;


QSharedPointer<MockDataChannel> MockIfdServer::createDataChannel()
{
	const auto& dataChannel = QSharedPointer<MockDataChannel>::create();
	connect(dataChannel.data(), &MockDataChannel::destroyed, this, &MockIfdServer::fireDataChannelDestroyed);
	return dataChannel;
}


MockIfdServer::MockIfdServer(bool pIsLocal)
	: IfdServer()
	, mRunning(false)
	, mConnected(false)
	, mPairing(false)
	, mIsLocal(pIsLocal)
	, mServerMessageHandler(new ServerMessageHandlerImpl(createDataChannel()))
{
}


bool MockIfdServer::isRunning() const
{
	return mRunning;
}


bool MockIfdServer::start(const QString& pServerName)
{
	Q_UNUSED(pServerName)
	mRunning = true;
	Q_EMIT fireIsRunningChanged();
	return true;
}


void MockIfdServer::stop()
{
	mRunning = false;
	mServerMessageHandler.reset();
	Q_EMIT fireIsRunningChanged();
}


void MockIfdServer::setPairing(bool pEnable)
{
	mPairing = pEnable;
}


bool MockIfdServer::getPairing()
{
	return mPairing;
}


bool MockIfdServer::isConnected() const
{
	return mConnected;
}


bool MockIfdServer::isPairingConnection() const
{
	return false;
}


void MockIfdServer::setConnected(bool pConnected)
{
	mConnected = pConnected;
}


QSslCertificate MockIfdServer::getCurrentCertificate() const
{
	return QSslCertificate(QByteArray(), QSsl::Pem);
}


const QSharedPointer<ServerMessageHandler>& MockIfdServer::getMessageHandler() const
{
	return mServerMessageHandler;
}


void MockIfdServer::setMessageHandler(const QSharedPointer<ServerMessageHandler>& pHandler)
{
	mServerMessageHandler = pHandler;
}
