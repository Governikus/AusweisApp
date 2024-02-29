/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "IfdServiceContext.h"


using namespace governikus;


bool IfdServiceContext::isPaceRequestingRights() const
{
	return !mEstablishPaceChannel.getChat().isEmpty() && !mEstablishPaceChannel.getCertificateDescription().isEmpty();
}


void IfdServiceContext::onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler) const
{
	connect(pHandler.data(), &ServerMessageHandler::fireCardConnected, this, &IfdServiceContext::fireCardConnected);
	connect(pHandler.data(), &ServerMessageHandler::fireCardDisconnected, this, &IfdServiceContext::fireCardDisconnected);
	connect(pHandler.data(), &ServerMessageHandler::fireClosed, this, &IfdServiceContext::reset);
	connect(getIfdServer().data(), &IfdServer::fireConnectedChanged, this, &IfdServiceContext::reset);
}


IfdServiceContext::IfdServiceContext(const QSharedPointer<IfdServer>& pIfdServer)
	: WorkflowContext(Action::REMOTE_SERVICE)
	, mIfdServer(pIfdServer)
	, mNewPin()
	, mSlotHandle()
	, mDisplayText()
	, mEstablishPaceChannel()
	, mRequestTransportPin(false)
	, mAllowToChangePinLength(false)
	, mEstablishPaceChannelOutput()
	, mAccessRightManager()
	, mModifyPinMessage()
	, mModifyPinMessageResponseApdu()
{
	Q_ASSERT(mIfdServer);

	if (const auto& messageHandler = mIfdServer->getMessageHandler(); messageHandler)
	{
		onMessageHandlerAdded(messageHandler);
	}

	connect(mIfdServer.data(), &IfdServer::fireIsRunningChanged, this, &IfdServiceContext::fireIsRunningChanged);
	connect(mIfdServer.data(), &IfdServer::fireMessageHandlerAdded, this, &IfdServiceContext::onMessageHandlerAdded);
}


IfdServiceContext::~IfdServiceContext()
{
#ifndef QT_NO_DEBUG
	if (!QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		Q_ASSERT(getNewPin().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	}
#endif
}


const QSharedPointer<IfdServer>& IfdServiceContext::getIfdServer() const
{
	Q_ASSERT(mIfdServer);
	return mIfdServer;
}


bool IfdServiceContext::isRunning() const
{
	Q_ASSERT(mIfdServer);
	return mIfdServer ? mIfdServer->isRunning() : false;
}


const QString& IfdServiceContext::getNewPin() const
{
	return mNewPin;
}


void IfdServiceContext::setNewPin(const QString& pNewPin)
{
	mNewPin = pNewPin;
}


bool IfdServiceContext::isPinChangeWorkflow() const
{
	return mEstablishPaceChannel.getPasswordId() == PacePasswordId::PACE_PIN && !isPaceRequestingRights();
}


bool IfdServiceContext::isPinAuthentication() const
{
	return mEstablishPaceChannel.getPasswordId() == PacePasswordId::PACE_PIN && isPaceRequestingRights();
}


bool IfdServiceContext::isCanAllowedMode() const
{
	return mEstablishPaceChannel.getPasswordId() == PacePasswordId::PACE_CAN && isPaceRequestingRights();
}


void IfdServiceContext::cancelPasswordRequest()
{
	Q_EMIT fireCancelPasswordRequest();
}


void IfdServiceContext::setEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage)
{
	if (pMessage)
	{
		mSlotHandle = pMessage->getSlotHandle();
		mEstablishPaceChannel = pMessage->getInputData();
		mRequestTransportPin = pMessage->getExpectedPinLength() == 5;
		mAllowToChangePinLength = isPinChangeWorkflow() && pMessage->getExpectedPinLength() == 0;
		const auto& chat = mEstablishPaceChannel.getChat();
		if (!chat.isEmpty())
		{
			const auto& requiredChat = CHAT::decode(chat);
			mAccessRightManager = QSharedPointer<AccessRightManager>::create(requiredChat);
			Q_EMIT fireAccessRightManagerCreated(mAccessRightManager);
		}
	}
	else
	{
		mSlotHandle.clear();
		mAccessRightManager.clear();
		mEstablishPaceChannel = EstablishPaceChannel();
		mRequestTransportPin = false;
		mAllowToChangePinLength = false;
	}
	mEstablishPaceChannelOutput = EstablishPaceChannelOutput();

	setEstablishPaceChannelType(mEstablishPaceChannel.getPasswordId());

	Q_EMIT fireCanAllowedModeChanged();
	Q_EMIT fireEstablishPaceChannelUpdated();
}


const QString& IfdServiceContext::getSlotHandle() const
{
	return mSlotHandle;
}


void IfdServiceContext::setDisplayText(const QString& pDisplayText)
{
	if (mDisplayText != pDisplayText)
	{
		mDisplayText = pDisplayText;
		Q_EMIT fireDisplayTextChanged();
	}
}


const QString& IfdServiceContext::getDisplayText() const
{
	return mDisplayText;
}


const EstablishPaceChannel& IfdServiceContext::getEstablishPaceChannel() const
{
	return mEstablishPaceChannel;
}


QSharedPointer<AccessRightManager> IfdServiceContext::getAccessRightManager() const
{
	return mAccessRightManager;
}


QSharedPointer<const CertificateDescription> IfdServiceContext::getCertificateDescription() const
{
	const auto& certDescription = mEstablishPaceChannel.getCertificateDescription();
	if (certDescription.isEmpty())
	{
		return QSharedPointer<const CertificateDescription>();
	}

	return CertificateDescription::decode(certDescription);
}


void IfdServiceContext::changePinLength()
{
	if (isPinChangeWorkflow())
	{
		mRequestTransportPin = !mRequestTransportPin;
		Q_EMIT firePasswordTypeChanged();
		return;
	}
}


bool IfdServiceContext::allowToChangePinLength() const
{
	return mAllowToChangePinLength;
}


bool IfdServiceContext::isRequestTransportPin() const
{
	return mRequestTransportPin;
}


void IfdServiceContext::setEstablishPaceChannelOutput(const EstablishPaceChannelOutput& pEstablishPaceChannelOutput)
{
	mEstablishPaceChannelOutput = pEstablishPaceChannelOutput;
}


const EstablishPaceChannelOutput& IfdServiceContext::getEstablishPaceChannelOutput() const
{
	return mEstablishPaceChannelOutput;
}


void IfdServiceContext::setModifyPinMessage(const QSharedPointer<const IfdModifyPin>& pMessage)
{
	mModifyPinMessage = pMessage;
	mModifyPinMessageResponseApdu = ResponseApdu();
	Q_EMIT firePasswordTypeChanged();
}


const QSharedPointer<const IfdModifyPin>& IfdServiceContext::getModifyPinMessage() const
{
	return mModifyPinMessage;
}


void IfdServiceContext::setModifyPinMessageResponseApdu(const ResponseApdu& pModifyPinMessageResponseApdu)
{
	mModifyPinMessageResponseApdu = pModifyPinMessageResponseApdu;
}


const ResponseApdu& IfdServiceContext::getModifyPinMessageResponseApdu() const
{
	return mModifyPinMessageResponseApdu;
}


bool IfdServiceContext::modifyPinRunning() const
{
	return mModifyPinMessage && mModifyPinMessage->isValid();
}


void IfdServiceContext::resetPacePasswords()
{
	setNewPin(QString());
	WorkflowContext::resetPacePasswords();
}


void IfdServiceContext::reset()
{
	qDebug() << "Resetting all PACE passwords and further relevant context information.";

	setDisplayText(QString());
	resetPacePasswords();
	resetCardConnection();
	resetLastPaceResult();
	setEstablishPaceChannel(QSharedPointer<const IfdEstablishPaceChannel>());
	mModifyPinMessage = QSharedPointer<const IfdModifyPin>();
}


[[nodiscard]] QList<AcceptedEidType> IfdServiceContext::getAcceptedEidTypes() const
{
	return {AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::SE_CERTIFIED, AcceptedEidType::SE_ENDORSED, AcceptedEidType::HW_KEYSTORE};
}
