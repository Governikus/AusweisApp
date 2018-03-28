/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceContext.h"

#include "Env.h"


using namespace governikus;

RemoteServiceContext::RemoteServiceContext()
	: mRemoteServer(Env::create<RemoteServer*>())
	, mNewPin()
	, mEstablishPaceChannelMessage()
	, mModifyPinMessage()
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


const QString& RemoteServiceContext::getNewPin() const
{
	return mNewPin;
}


void RemoteServiceContext::setNewPin(const QString& pNewPin)
{
	mNewPin = pNewPin;
}


void RemoteServiceContext::setEstablishPaceChannelMessage(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage)
{
	mEstablishPaceChannelMessage = pMessage;
}


const QSharedPointer<const IfdEstablishPaceChannel>& RemoteServiceContext::getEstablishPaceChannelMessage() const
{
	return mEstablishPaceChannelMessage;
}


void RemoteServiceContext::setModifyPinMessage(const QSharedPointer<const IfdModifyPin>& pMessage)
{
	mModifyPinMessage = pMessage;
}


const QSharedPointer<const IfdModifyPin>& RemoteServiceContext::getModifyPinMessage() const
{
	return mModifyPinMessage;
}


void RemoteServiceContext::onResetMessageHandler()
{
	setCardConnection(QSharedPointer<CardConnection>());
	setCan(QString());
	setPin(QString());
	setPuk(QString());
	setNewPin(QString());
	resetLastPaceResultAndRetryCounter();
	mEstablishPaceChannelMessage = QSharedPointer<const IfdEstablishPaceChannel>();
	mModifyPinMessage = QSharedPointer<const IfdModifyPin>();
}
