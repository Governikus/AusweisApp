/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceContext.h"

#include "Env.h"


using namespace governikus;

RemoteServiceContext::RemoteServiceContext()
	: mRemoteServer(Env::create<RemoteServer*>())
	, mEstablishPaceChannelMessage()
{
}


RemoteServiceContext::~RemoteServiceContext()
{
}


const QSharedPointer<RemoteServer>& RemoteServiceContext::getRemoteServer() const
{
	Q_ASSERT(mRemoteServer);
	return mRemoteServer;
}


bool RemoteServiceContext::isRunning() const
{
	Q_ASSERT(mRemoteServer);
	return mRemoteServer ? mRemoteServer->isRunning() : false;
}


void RemoteServiceContext::setEstablishPaceChannelMessage(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage)
{
	mEstablishPaceChannelMessage = pMessage;
}


const QSharedPointer<const IfdEstablishPaceChannel>& RemoteServiceContext::getEstablishPaceChannelMessage() const
{
	return mEstablishPaceChannelMessage;
}


void RemoteServiceContext::onResetMessageHandler()
{
	setCardConnection(QSharedPointer<CardConnection>());
	setCan(QString());
	setPin(QString());
	setPuk(QString());
	resetLastPaceResultAndRetryCounter();
	mEstablishPaceChannelMessage = QSharedPointer<const IfdEstablishPaceChannel>();
}
