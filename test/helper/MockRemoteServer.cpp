/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "MockRemoteServer.h"

using namespace governikus;

MockRemoteServer::MockRemoteServer()
	: RemoteServer()
	, mRunning(false)
	, mConnected(false)
	, mPairing(false)
	, mServerMessageHandler(QSharedPointer<ServerMessageHandlerImpl>(new ServerMessageHandlerImpl(nullptr)))
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
	return true;
}


void MockRemoteServer::stop()
{
	mRunning = false;
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
