/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteCard.h"

#include "EstablishPaceChannel.h"
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
#include "PinModify.h"
#include "PinModifyOutput.h"

#include <QLoggingCategory>
#include <QMutexLocker>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_remote)


bool RemoteCard::sendMessage(const QSharedPointer<const RemoteMessage>& pMessage, RemoteCardMessageType pExpectedAnswer, unsigned long pTimeout)
{
	// mResponseAvailable is locked by the constructor, to revert the mutex behavior.
	// Locking this is a requirement for QWaitCondition.

	mWaitingForAnswer = true;
	mExpectedAnswerType = pExpectedAnswer;

	const auto& connectionMR = QObject::connect(mRemoteDispatcher.data(), &RemoteDispatcher::fireReceived, this, &RemoteCard::onMessageReceived, Qt::DirectConnection);
	const auto& connectionDC = QObject::connect(mRemoteDispatcher.data(), &RemoteDispatcherClient::fireClosed, this, &RemoteCard::onDispatcherClosed, Qt::DirectConnection);
	const auto& localCopy = mRemoteDispatcher;
	QMetaObject::invokeMethod(localCopy.data(), [localCopy, pMessage] {
				localCopy->send(pMessage);
			}, Qt::QueuedConnection);

	mWaitCondition.wait(&mResponseAvailable, pTimeout);
	QObject::disconnect(connectionMR);
	QObject::disconnect(connectionDC);

	QMutexLocker locker(&mProcessResponse);

	if (mWaitingForAnswer)
	{
		mWaitingForAnswer = false;
		return false;
	}

	return true;
}


void RemoteCard::onMessageReceived(RemoteCardMessageType pMessageTpe, const QJsonObject pJsonObject)
{
	QMutexLocker locker(&mProcessResponse);

	if (!mWaitingForAnswer)
	{
		return;
	}

	if (pMessageTpe == mExpectedAnswerType || pMessageTpe == RemoteCardMessageType::IFDError)
	{
		mResponse = pJsonObject;
		mWaitingForAnswer = false;
		mWaitCondition.wakeOne();
	}
}


void RemoteCard::onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId)
{
	Q_UNUSED(pId);
	QMutexLocker locker(&mProcessResponse);

	if (mWaitingForAnswer)
	{
		qCWarning(card_remote) << "RemoteDispatcher was closed while waiting for an answer:" << pCloseCode;

		mResponse = QJsonObject();
		mWaitingForAnswer = false;
		mWaitCondition.wakeOne();
	}
}


RemoteCard::RemoteCard(const QSharedPointer<RemoteDispatcherClient>& pRemoteDispatcher, const QString& pReaderName)
	: Card()
	, mWaitingForAnswer(false)
	, mWaitCondition()
	, mResponseAvailable()
	, mProcessResponse()
	, mExpectedAnswerType()
	, mResponse()
	, mRemoteDispatcher(pRemoteDispatcher)
	, mReaderName(pReaderName)
	, mConnected(false)
{
	Q_ASSERT(mRemoteDispatcher);

	mResponseAvailable.lock();
	const QString& contextHandle = mRemoteDispatcher->getContextHandle();
	mReaderName.remove(contextHandle);
}


RemoteCard::~RemoteCard()
{
	mResponseAvailable.unlock();
}


CardReturnCode RemoteCard::connect()
{
	const auto& connectMsg = QSharedPointer<IfdConnect>::create(mReaderName);
	if (sendMessage(connectMsg, RemoteCardMessageType::IFDConnectResponse, 5000))
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


CardReturnCode RemoteCard::disconnect()
{
	const auto& disconnectCmd = QSharedPointer<IfdDisconnect>::create(mSlotHandle);
	if (sendMessage(disconnectCmd, RemoteCardMessageType::IFDDisconnectResponse, 5000))
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


bool RemoteCard::isConnected()
{
	return mConnected;
}


CardReturnCode RemoteCard::transmit(const CommandApdu& pCommand, ResponseApdu& pResponse)
{
	const auto& transmitCmd = QSharedPointer<const IfdTransmit>::create(mSlotHandle, pCommand.getBuffer());
	if (sendMessage(transmitCmd, RemoteCardMessageType::IFDTransmitResponse, 5000))
	{
		const IfdTransmitResponse response(mResponse);
		if (!response.isIncomplete())
		{
			if (!response.resultHasError())
			{
				pResponse.setBuffer(response.getResponseApdu());
				return CardReturnCode::OK;
			}
			qCWarning(card_remote) << response.getResultMinor();
		}

		return CardReturnCode::COMMAND_FAILED;
	}

	return CardReturnCode::INPUT_TIME_OUT;
}


CardReturnCode RemoteCard::establishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPaceChannelOutput& pChannelOutput, quint8 pTimeoutSeconds)
{
	EstablishPaceChannel builder;
	builder.setPasswordId(pPasswordId);
	builder.setChat(pChat);
	builder.setCertificateDescription(pCertificateDescription);
	const QByteArray inputData = builder.createCommandDataCcid().getBuffer();

	const auto& message = QSharedPointer<const IfdEstablishPaceChannel>::create(mSlotHandle, inputData);
	if (sendMessage(message, RemoteCardMessageType::IFDEstablishPACEChannelResponse, pTimeoutSeconds * 1000))
	{
		const IfdEstablishPaceChannelResponse response(mResponse);
		if (!response.isIncomplete())
		{
			if (response.getResultMinor() == ECardApiResult::Minor::IFDL_Terminal_NoCard)
			{
				return CardReturnCode::CARD_NOT_FOUND;
			}

			if (!response.resultHasError())
			{
				pChannelOutput.parseFromCcid(response.getOutputData(), pPasswordId);
				return pChannelOutput.getPaceReturnCode();
			}
			qCWarning(card_remote) << response.getResultMinor();
		}

		return CardReturnCode::COMMAND_FAILED;
	}

	return CardReturnCode::INPUT_TIME_OUT;
}


CardReturnCode RemoteCard::setEidPin(quint8 pTimeoutSeconds, ResponseApdu& pResponseApdu)
{
	PinModify pinModify(pTimeoutSeconds);
	const QByteArray inputData = pinModify.createCcidForRemote();

	const auto& message = QSharedPointer<const IfdModifyPin>::create(mSlotHandle, inputData);
	if (sendMessage(message, RemoteCardMessageType::IFDModifyPINResponse, pTimeoutSeconds * 1000))
	{
		const IfdModifyPinResponse response(mResponse);
		if (response.resultHasError())
		{
			return response.getReturnCode();
		}

		if (!response.isIncomplete())
		{
			PinModifyOutput output(response.getOutputData());
			pResponseApdu.setBuffer(output.getResponseApdu().getBuffer());
			if (!response.resultHasError())
			{
				return output.getReturnCode();
			}
		}

		return CardReturnCode::COMMAND_FAILED;
	}

	return CardReturnCode::INPUT_TIME_OUT;
}
