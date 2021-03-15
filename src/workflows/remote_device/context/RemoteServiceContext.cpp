/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceContext.h"

#include "Env.h"


using namespace governikus;


bool RemoteServiceContext::isPaceRequestingRights() const
{
	return !mEstablishPaceChannel.getChat().isEmpty() && !mEstablishPaceChannel.getCertificateDescription().isEmpty();
}


void RemoteServiceContext::onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler)
{
	connect(pHandler.data(), &ServerMessageHandler::fireCardConnectionEstablished, this, &RemoteServiceContext::fireCardConnectionEstablished);
	connect(getRemoteServer().data(), &RemoteServer::fireConnectedChanged, this, &RemoteServiceContext::onResetMessageHandler);
}


RemoteServiceContext::RemoteServiceContext()
	: mRemoteServer(Env::create<RemoteServer*>())
	, mNewPin()
	, mSlotHandle()
	, mEstablishPaceChannel()
	, mPreferredPinLength(0)
	, mEstablishPaceChannelOutput()
	, mModifyPinMessage()
	, mModifyPinMessageResponseApdu()
{
	connect(mRemoteServer.data(), &RemoteServer::fireIsRunningChanged, this, &RemoteServiceContext::fireIsRunningChanged);
	connect(mRemoteServer.data(), &RemoteServer::fireMessageHandlerAdded, this, &RemoteServiceContext::onMessageHandlerAdded);
}


RemoteServiceContext::~RemoteServiceContext()
{
#ifndef QT_NO_DEBUG
	if (!QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		Q_ASSERT(getNewPin().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	}
#endif
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


bool RemoteServiceContext::isPinChangeWorkflow() const
{
	return mEstablishPaceChannel.getPasswordId() == PacePasswordId::PACE_PIN && !isPaceRequestingRights();
}


bool RemoteServiceContext::isCanAllowedMode() const
{
	return mEstablishPaceChannel.getPasswordId() == PacePasswordId::PACE_CAN && isPaceRequestingRights();
}


void RemoteServiceContext::setEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage)
{
	if (pMessage)
	{
		mSlotHandle = pMessage->getSlotHandle();
		mEstablishPaceChannel = pMessage->getInputData();
		mPreferredPinLength = pMessage->getPreferredPinLength();
	}
	else
	{
		mSlotHandle.clear();
		mEstablishPaceChannel = EstablishPaceChannel();
		mPreferredPinLength = 0;
	}
	mEstablishPaceChannelOutput = EstablishPaceChannelOutput();

	Q_EMIT fireCanAllowedModeChanged();
	Q_EMIT fireEstablishPaceChannelUpdated();
}


const QString& RemoteServiceContext::getSlotHandle() const
{
	return mSlotHandle;
}


const EstablishPaceChannel& RemoteServiceContext::getEstablishPaceChannel() const
{
	return mEstablishPaceChannel;
}


int RemoteServiceContext::getPreferredPinLength() const
{
	return mPreferredPinLength;
}


void RemoteServiceContext::setEstablishPaceChannelOutput(const EstablishPaceChannelOutput& pEstablishPaceChannelOutput)
{
	mEstablishPaceChannelOutput = pEstablishPaceChannelOutput;
}


const EstablishPaceChannelOutput& RemoteServiceContext::getEstablishPaceChannelOutput() const
{
	return mEstablishPaceChannelOutput;
}


void RemoteServiceContext::setModifyPinMessage(const QSharedPointer<const IfdModifyPin>& pMessage)
{
	mModifyPinMessage = pMessage;
	mModifyPinMessageResponseApdu = ResponseApdu();
}


const QSharedPointer<const IfdModifyPin>& RemoteServiceContext::getModifyPinMessage() const
{
	return mModifyPinMessage;
}


void RemoteServiceContext::setModifyPinMessageResponseApdu(const ResponseApdu& pModifyPinMessageResponseApdu)
{
	mModifyPinMessageResponseApdu = pModifyPinMessageResponseApdu;
}


const ResponseApdu& RemoteServiceContext::getModifyPinMessageResponseApdu() const
{
	return mModifyPinMessageResponseApdu;
}


void RemoteServiceContext::resetPacePasswords()
{
	setNewPin(QString());
	WorkflowContext::resetPacePasswords();
}


void RemoteServiceContext::onResetMessageHandler()
{
	resetPacePasswords();
	resetCardConnection();
	resetLastPaceResult();
	setEstablishPaceChannel(QSharedPointer<const IfdEstablishPaceChannel>());
	mModifyPinMessage = QSharedPointer<const IfdModifyPin>();
}
