/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceContext.h"

#include "Env.h"


using namespace governikus;


void RemoteServiceContext::onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler)
{
	connect(pHandler.data(), &ServerMessageHandler::fireCardConnectionEstablished, this, &RemoteServiceContext::fireCardConnectionEstablished);
	connect(getRemoteServer().data(), &RemoteServer::fireConnectedChanged, this, &RemoteServiceContext::onResetMessageHandler);
}


RemoteServiceContext::RemoteServiceContext()
	: mRemoteServer(Env::create<RemoteServer*>())
	, mNewPin()
	, mEstablishPaceChannelMessage()
	, mEstablishPaceChannelOutput()
	, mModifyPinMessage()
	, mModifyPinMessageResponseApdu()
	, mPaceChannelParser()
{
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


bool RemoteServiceContext::isCanAllowedMode() const
{
	if (mPaceChannelParser.getPasswordId() != PacePasswordId::PACE_CAN)
	{
		return false;
	}

	return !mPaceChannelParser.getChat().isEmpty() && !mPaceChannelParser.getCertificateDescription().isEmpty();
}


void RemoteServiceContext::setEstablishPaceChannelMessage(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage)
{
	mEstablishPaceChannelMessage = pMessage;
	mEstablishPaceChannelOutput = EstablishPaceChannelOutput();

	if (!pMessage)
	{
		qDebug() << "PaceChannelMessage is empty";
		mPaceChannelParser.reset();
		return;
	}

	mPaceChannelParser.fromCcid(pMessage->getInputData());

	Q_EMIT fireCanAllowedModeChanged();
	Q_EMIT fireEstablishPaceChannelMessageUpdated(mEstablishPaceChannelMessage);
}


bool RemoteServiceContext::hasEstablishedPaceChannelRequest() const
{
	return mEstablishPaceChannelMessage && !mEstablishPaceChannelMessage->isIncomplete();
}


QString RemoteServiceContext::getEstablishPaceChannelMessageSlotHandle() const
{
	if (!hasEstablishedPaceChannelRequest())
	{
		return QString();
	}

	return mEstablishPaceChannelMessage->getSlotHandle();
}


const EstablishPaceChannelParser& RemoteServiceContext::getPaceChannelParser() const
{
	return mPaceChannelParser;
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
	setEstablishPaceChannelMessage(QSharedPointer<const IfdEstablishPaceChannel>());
	mModifyPinMessage = QSharedPointer<const IfdModifyPin>();
}
