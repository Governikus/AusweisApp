/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MockIfdServer.h"

#include "MockDataChannel.h"
#include "ServerMessageHandlerImpl.h"

using namespace governikus;

MockIfdServer::MockIfdServer(bool pIsLocal)
	: IfdServer()
	, mRunning(false)
	, mConnected(false)
	, mPairing(false)
	, mIsLocal(pIsLocal)
	, mServerMessageHandler(QSharedPointer<ServerMessageHandlerImpl>(new ServerMessageHandlerImpl(QSharedPointer<MockDataChannel>::create())))
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
