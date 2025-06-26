/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdCard.h"

#include "VolatileSettings.h"
#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDestroyPaceChannel.h"
#include "messages/IfdDestroyPaceChannelResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdEstablishPaceChannelResponse.h"
#include "messages/IfdModifyPin.h"
#include "messages/IfdModifyPinResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"
#include "pinpad/EstablishPaceChannel.h"
#include "pinpad/PinModify.h"

#include <QLoggingCategory>
#include <QMutexLocker>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_remote)


bool IfdCard::sendMessage(const QSharedPointer<const IfdMessage>& pMessage, IfdMessageType pExpectedAnswer, unsigned long pExtraTimeout)
{
	// mResponseAvailable is locked by the constructor, to revert the mutex behavior.
	// Locking this is a requirement for QWaitCondition.

	mCardRemoved = false;
	mWaitingForAnswer = true;
	mExpectedAnswerType = pExpectedAnswer;

	const auto& connectionMR = connect(mDispatcher.data(), &IfdDispatcher::fireReceived, this, &IfdCard::onMessageReceived, Qt::DirectConnection);
	const auto& connectionDC = connect(mDispatcher.data(), &IfdDispatcherClient::fireClosed, this, &IfdCard::onDispatcherClosed, Qt::DirectConnection);
	const auto& localCopy = mDispatcher;
	QMetaObject::invokeMethod(localCopy.data(), [localCopy, pMessage] {
				localCopy->send(pMessage);
			}, Qt::QueuedConnection);

	const auto responseTimeout =
#ifndef QT_NO_DEBUG
			qEnvironmentVariableIsSet("SAC_RESPONSE_TIMEOUT") ? qgetenv("SAC_RESPONSE_TIMEOUT").toUInt() :
#endif
			5000 + pExtraTimeout;
	mWaitCondition.wait(&mResponseAvailable, responseTimeout);
	disconnect(connectionMR);
	disconnect(connectionDC);

	QMutexLocker locker(&mProcessResponse);

	if (mWaitingForAnswer)
	{
		qCDebug(card_remote) << "Expected answer (" << pExpectedAnswer << ") was not received within" << responseTimeout << "ms.";
		mWaitingForAnswer = false;
		return false;
	}

	return true;
}


void IfdCard::onMessageReceived(IfdMessageType pMessageType, const QJsonObject& pJsonObject)
{
	QMutexLocker locker(&mProcessResponse);

	if (!mWaitingForAnswer)
	{
		return;
	}

	if (pMessageType == mExpectedAnswerType || pMessageType == IfdMessageType::IFDError)
	{
		mResponse = pJsonObject;
		mWaitingForAnswer = false;
		mWaitCondition.wakeOne();
		return;
	}

	if (pMessageType == IfdMessageType::IFDStatus)
	{
		const IfdStatus ifdStatus(pJsonObject);
		if (!ifdStatus.getCardAvailable())
		{
			qCDebug(card_remote) << "Card was removed while waiting for" << mExpectedAnswerType;
			mCardRemoved = true;
			return;
		}
	}

	qCWarning(card_remote) << "Ignoring unexpected message type:" << pMessageType;
}


void IfdCard::onDispatcherClosed(GlobalStatus::Code pCloseCode, const QByteArray& pId)
{
	Q_UNUSED(pId)
	QMutexLocker locker(&mProcessResponse);

	if (mWaitingForAnswer)
	{
		qCWarning(card_remote) << "IfdDispatcher was closed while waiting for an answer:" << pCloseCode;

		mResponse = QJsonObject();
		mWaitingForAnswer = false;
		mWaitCondition.wakeOne();
	}
}


IfdCard::IfdCard(const QSharedPointer<IfdDispatcherClient>& pDispatcher, const QString& pReaderName)
	: Card()
	, mWaitingForAnswer(false)
	, mWaitCondition()
	, mResponseAvailable()
	, mProcessResponse()
	, mExpectedAnswerType()
	, mResponse()
	, mDispatcher(pDispatcher)
	, mReaderName(pReaderName)
	, mConnected(false)
	, mProgressMessage()
	, mCardRemoved(false)
{
	Q_ASSERT(mDispatcher);

	mResponseAvailable.lock();
	const QString& contextHandle = mDispatcher->getContextHandle();
	mReaderName.remove(contextHandle);
}


IfdCard::~IfdCard()
{
	mResponseAvailable.unlock();
}


CardReturnCode IfdCard::establishConnection()
{
	const auto& connectMsg = QSharedPointer<IfdConnect>::create(mReaderName);
	if (!sendMessage(connectMsg, IfdMessageType::IFDConnectResponse))
	{
		return CardReturnCode::INPUT_TIME_OUT;
	}

	const IfdConnectResponse response(mResponse);
	if (response.isIncomplete())
	{
		return CardReturnCode::COMMAND_FAILED;
	}
	if (response.resultHasError())
	{
		qCWarning(card_remote) << response.getResultMinor();
		return CardReturnCode::COMMAND_FAILED;
	}

	mConnected = true;
	mSlotHandle = response.getSlotHandle();
	return CardReturnCode::OK;
}


CardReturnCode IfdCard::releaseConnection()
{
	const auto& disconnectCmd = QSharedPointer<IfdDisconnect>::create(mSlotHandle);
	if (!sendMessage(disconnectCmd, IfdMessageType::IFDDisconnectResponse))
	{
		return CardReturnCode::INPUT_TIME_OUT;
	}

	const IfdDisconnectResponse response(mResponse);
	if (response.isIncomplete())
	{
		return CardReturnCode::COMMAND_FAILED;
	}
	if (response.resultHasError())
	{
		qCWarning(card_remote) << response.getResultMinor();
		return CardReturnCode::COMMAND_FAILED;
	}

	mConnected = false;
	return CardReturnCode::OK;
}


bool IfdCard::isConnected() const
{
	return mConnected;
}


void IfdCard::setProgressMessage(const QString& pMessage, int pProgress)
{
	mProgressMessage = generateProgressMessage(pMessage, pProgress);
}


void IfdCard::setErrorMessage(const QString& pMessage)
{
	mProgressMessage = generateErrorMessage(pMessage);
}


ResponseApduResult IfdCard::transmit(const CommandApdu& pCommand)
{
	qCDebug(card_remote) << "Transmit command APDU:" << pCommand;

	const QSharedPointer<const IfdTransmit>& transmitCmd = QSharedPointer<IfdTransmit>::create(mSlotHandle, pCommand, mProgressMessage);
	if (!sendMessage(transmitCmd, IfdMessageType::IFDTransmitResponse))
	{
		return {CardReturnCode::INPUT_TIME_OUT};
	}

	mProgressMessage.clear();
	const IfdTransmitResponse response(mResponse);
	if (response.isIncomplete())
	{
		return {CardReturnCode::COMMAND_FAILED};
	}
	if (response.resultHasError())
	{
		qCWarning(card_remote) << response.getResultMinor();
		return {CardReturnCode::COMMAND_FAILED};
	}

	const ResponseApdu responseApdu(response.getResponseApdu());
	qCDebug(card_remote) << "Transmit response APDU:" << responseApdu;
	return {CardReturnCode::OK, responseApdu};
}


EstablishPaceChannelOutput IfdCard::establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription)
{
	EstablishPaceChannel establishPaceChannel(pPasswordId, pChat, pCertificateDescription);
	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		pPreferredPinLength = 0;
	}

	const QSharedPointer<const IfdEstablishPaceChannel>& message = QSharedPointer<IfdEstablishPaceChannel>::create(mSlotHandle, establishPaceChannel, pPreferredPinLength);
	if (!sendMessage(message, IfdMessageType::IFDEstablishPACEChannelResponse, DEFAULT_PINPAD_TIMEOUT * 1000))
	{
		return EstablishPaceChannelOutput(CardReturnCode::INPUT_TIME_OUT);
	}

	const IfdEstablishPaceChannelResponse response(mResponse);
	if (response.isIncomplete())
	{
		return EstablishPaceChannelOutput(CardReturnCode::COMMAND_FAILED);
	}
	if (response.getResultMinor() == ECardApiResult::Minor::IFDL_Terminal_NoCard
			|| response.getResultMinor() == ECardApiResult::Minor::IFDL_InvalidSlotHandle)
	{
		return EstablishPaceChannelOutput(CardReturnCode::CARD_NOT_FOUND);
	}
	if (response.resultHasError())
	{
		qCWarning(card_remote) << response.getResultMinor();
		return EstablishPaceChannelOutput(response.getReturnCode());
	}

	if (mCardRemoved && response.getOutputData().getPaceReturnCode() == CardReturnCode::OK)
	{
		return EstablishPaceChannelOutput(CardReturnCode::CARD_NOT_FOUND);
	}

	return response.getOutputData();

}


CardReturnCode IfdCard::destroyPaceChannel()
{
	const auto& destroyCmd = QSharedPointer<IfdDestroyPaceChannel>::create(mSlotHandle);
	if (!sendMessage(destroyCmd, IfdMessageType::IFDDestroyPACEChannelResponse))
	{
		return CardReturnCode::INPUT_TIME_OUT;
	}

	const IfdDestroyPaceChannelResponse response(mResponse);
	if (response.isIncomplete())
	{
		return CardReturnCode::COMMAND_FAILED;
	}
	if (response.resultHasError())
	{
		qCWarning(card_remote) << response.getResultMinor();
		return CardReturnCode::COMMAND_FAILED;
	}

	return CardReturnCode::OK;
}


ResponseApduResult IfdCard::setEidPin(quint8 pTimeoutSeconds)
{
	PinModify pinModify(pTimeoutSeconds);
	const QByteArray inputData = pinModify.createCcid();

	const QSharedPointer<const IfdModifyPin>& message = QSharedPointer<IfdModifyPin>::create(mSlotHandle, inputData);
	if (!sendMessage(message, IfdMessageType::IFDModifyPINResponse, pTimeoutSeconds * 1000))
	{
		return {CardReturnCode::INPUT_TIME_OUT};
	}

	const IfdModifyPinResponse response(mResponse);
	if (response.isIncomplete())
	{
		return {CardReturnCode::COMMAND_FAILED};
	}

	const ResponseApdu output(response.getOutputData());
	if (response.resultHasError())
	{
		qCWarning(card_remote) << response.getResultMinor();
		return {response.getReturnCode(), output};
	}

	if (mCardRemoved && output.getStatusCode() == StatusCode::SUCCESS)
	{
		return {CardReturnCode::CARD_NOT_FOUND};
	}

	return {CardReturnCode::OK, output};
}
