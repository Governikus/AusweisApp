/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "IfdCard.h"

#include "VolatileSettings.h"
#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdEstablishPaceChannelResponse.h"
#include "messages/IfdModifyPin.h"
#include "messages/IfdModifyPinResponse.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"
#include "pinpad/EstablishPaceChannel.h"
#include "pinpad/PinModify.h"
#include "pinpad/PinModifyOutput.h"

#include <QLoggingCategory>
#include <QMutexLocker>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_remote)


bool IfdCard::sendMessage(const QSharedPointer<const IfdMessage>& pMessage, IfdMessageType pExpectedAnswer, unsigned long pTimeout)
{
	// mResponseAvailable is locked by the constructor, to revert the mutex behavior.
	// Locking this is a requirement for QWaitCondition.

	mWaitingForAnswer = true;
	mExpectedAnswerType = pExpectedAnswer;

	const auto& connectionMR = connect(mDispatcher.data(), &IfdDispatcher::fireReceived, this, &IfdCard::onMessageReceived, Qt::DirectConnection);
	const auto& connectionDC = connect(mDispatcher.data(), &IfdDispatcherClient::fireClosed, this, &IfdCard::onDispatcherClosed, Qt::DirectConnection);
	const auto& localCopy = mDispatcher;
	QMetaObject::invokeMethod(localCopy.data(), [localCopy, pMessage] {
			localCopy->send(pMessage);
		}, Qt::QueuedConnection);

	mWaitCondition.wait(&mResponseAvailable, pTimeout);
	disconnect(connectionMR);
	disconnect(connectionDC);

	QMutexLocker locker(&mProcessResponse);

	if (mWaitingForAnswer)
	{
		mWaitingForAnswer = false;
		return false;
	}

	return true;
}


void IfdCard::onMessageReceived(IfdMessageType pMessageTpe, const QJsonObject& pJsonObject)
{
	QMutexLocker locker(&mProcessResponse);

	if (!mWaitingForAnswer)
	{
		return;
	}

	if (pMessageTpe == mExpectedAnswerType || pMessageTpe == IfdMessageType::IFDError)
	{
		mResponse = pJsonObject;
		mWaitingForAnswer = false;
		mWaitCondition.wakeOne();
	}
}


void IfdCard::onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId)
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
	if (sendMessage(connectMsg, IfdMessageType::IFDConnectResponse, 5000))
	{
		const IfdConnectResponse response(mResponse);
		if (!response.isIncomplete())
		{
			if (!response.resultHasError())
			{
				mConnected = true;
				mSlotHandle = response.getSlotHandle();
				return CardReturnCode::OK;
			}
			qCWarning(card_remote) << response.getResultMinor();
		}

		return CardReturnCode::COMMAND_FAILED;
	}

	return CardReturnCode::INPUT_TIME_OUT;
}


CardReturnCode IfdCard::releaseConnection()
{
	const auto& disconnectCmd = QSharedPointer<IfdDisconnect>::create(mSlotHandle);
	if (sendMessage(disconnectCmd, IfdMessageType::IFDDisconnectResponse, 5000))
	{
		const IfdDisconnectResponse response(mResponse);
		if (!response.isIncomplete())
		{
			if (!response.resultHasError())
			{
				mConnected = false;
				return CardReturnCode::OK;
			}
			qCWarning(card_remote) << response.getResultMinor();
		}

		return CardReturnCode::COMMAND_FAILED;
	}

	return CardReturnCode::INPUT_TIME_OUT;
}


bool IfdCard::isConnected() const
{
	return mConnected;
}


void IfdCard::setProgressMessage(const QString& pMessage, int pProgress)
{
	mProgressMessage = generateProgressMessage(pMessage, pProgress);
}


ResponseApduResult IfdCard::transmit(const CommandApdu& pCommand)
{
	qCDebug(card_remote) << "Transmit command APDU:" << pCommand;

	const QSharedPointer<const IfdTransmit>& transmitCmd = QSharedPointer<IfdTransmit>::create(mSlotHandle, pCommand, mProgressMessage);
	if (sendMessage(transmitCmd, IfdMessageType::IFDTransmitResponse, 5000))
	{
		mProgressMessage.clear();

		const IfdTransmitResponse response(mResponse);
		if (!response.isIncomplete())
		{
			if (!response.resultHasError())
			{
				qCDebug(card_remote) << "Transmit response APDU:" << response.getResponseApdu().toHex();
				return {CardReturnCode::OK, ResponseApdu(response.getResponseApdu())};
			}
			qCWarning(card_remote) << response.getResultMinor();
		}

		return {CardReturnCode::COMMAND_FAILED};
	}

	return {CardReturnCode::INPUT_TIME_OUT};
}


EstablishPaceChannelOutput IfdCard::establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription, quint8 pTimeoutSeconds)
{
	EstablishPaceChannel establishPaceChannel(pPasswordId, pChat, pCertificateDescription);

	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		pPreferredPinLength = 0;
	}

	const QSharedPointer<const IfdEstablishPaceChannel>& message = QSharedPointer<IfdEstablishPaceChannel>::create(mSlotHandle, establishPaceChannel, pPreferredPinLength);
	if (sendMessage(message, IfdMessageType::IFDEstablishPACEChannelResponse, pTimeoutSeconds * 1000))
	{
		const IfdEstablishPaceChannelResponse response(mResponse);
		if (!response.isIncomplete())
		{
			if (response.getResultMinor() == ECardApiResult::Minor::IFDL_Terminal_NoCard
					|| response.getResultMinor() == ECardApiResult::Minor::IFDL_InvalidSlotHandle)
			{
				return EstablishPaceChannelOutput(CardReturnCode::CARD_NOT_FOUND);
			}

			if (!response.resultHasError())
			{
				return response.getOutputData();
			}
			qCWarning(card_remote) << response.getResultMinor();
		}

		return EstablishPaceChannelOutput(CardReturnCode::COMMAND_FAILED);
	}

	return EstablishPaceChannelOutput(CardReturnCode::INPUT_TIME_OUT);
}


ResponseApduResult IfdCard::setEidPin(quint8 pTimeoutSeconds)
{
	PinModify pinModify(pTimeoutSeconds);
	const QByteArray inputData = pinModify.createCcid();

	const QSharedPointer<const IfdModifyPin>& message = QSharedPointer<IfdModifyPin>::create(mSlotHandle, inputData);
	if (sendMessage(message, IfdMessageType::IFDModifyPINResponse, pTimeoutSeconds * 1000))
	{
		const IfdModifyPinResponse response(mResponse);
		if (response.resultHasError())
		{
			return {response.getReturnCode()};
		}

		if (!response.isIncomplete())
		{
			const PinModifyOutput output(ResponseApdu(response.getOutputData()));
			if (!response.resultHasError())
			{
				return {CardReturnCode::OK, output.getResponseApdu()};
			}
			qCWarning(card_remote) << response.getResultMinor();
		}

		return {CardReturnCode::COMMAND_FAILED};
	}

	return {CardReturnCode::INPUT_TIME_OUT};
}
