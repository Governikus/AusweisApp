/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MockRemoteServer.h"

#include "MockDataChannel.h"

using namespace governikus;

MockRemoteServer::MockRemoteServer()
	: RemoteServer()
	, mRunning(false)
	, mConnected(false)
	, mPairing(false)
	, mServerMessageHandler(QSharedPointer<ServerMessageHandlerImpl>(new ServerMessageHandlerImpl(QSharedPointer<MockDataChannel>::create())))
{
}


bool MockRemoteServer::isRunning() const
{
	return mRunning;
}


bool MockRemoteServer::start(const QString& pServerName)
{
	Q_UNUSED(pServerName)
	mRunning = true;
	Q_EMIT fireIsRunningChanged();
	return true;
}


void MockRemoteServer::stop()
{
	mRunning = false;
	Q_EMIT fireIsRunningChanged();
}


void MockRemoteServer::setPairing(bool pEnable)
{
	mPairing = pEnable;
}


bool MockRemoteServer::getPairing()
{
	return mPairing;
}


bool MockRemoteServer::isConnected() const
{
	return mConnected;
}


bool MockRemoteServer::isPairingConnection() const
{
	return false;
}


void MockRemoteServer::setConnected(bool pConnected)
{
	mConnected = pConnected;
}


QSslCertificate MockRemoteServer::getCurrentCertificate() const
{
	return QSslCertificate(QByteArray(), QSsl::Pem);
}


const QSharedPointer<ServerMessageHandler>& MockRemoteServer::getMessageHandler() const
{
	return mServerMessageHandler;
}


void MockRemoteServer::setMessageHandler(const QSharedPointer<ServerMessageHandler>& pHandler)
{
	mServerMessageHandler = pHandler;
}
