/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteCard.h"

#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdError.h"
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

	const auto& connection = QObject::connect(mRemoteDispatcher.data(), &RemoteDispatcher::fireReceived, this, &RemoteCard::onMessageReceived, Qt::DirectConnection);
	QMetaObject::invokeMethod(mRemoteDispatcher.data(), "send", Qt::QueuedConnection, Q_ARG(QSharedPointer<const RemoteMessage>, pMessage));

	mWaitCondition.wait(&mResponseAvailable, pTimeout);
	QObject::disconnect(connection);

	QMutexLocker locker(&mProcessResponse);

	if (mWaitingForAnswer)
	{
		mWaitingForAnswer = false;
		return false;
	}

	return true;
}


void RemoteCard::onMessageReceived(const QSharedPointer<const RemoteMessage>& pMessage)
{
	QMutexLocker locker(&mProcessResponse);

	if (!mWaitingForAnswer)
	{
		return;
	}

	if (pMessage->getType() == mExpectedAnswerType || pMessage->getType() == RemoteCardMessageType::IFDError)
	{
		mResponse = pMessage;
		mWaitingForAnswer = false;
		mWaitCondition.wakeOne();
	}
}


void RemoteCard::onDispatcherClosed(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>&)
{
	QMutexLocker locker(&mProcessResponse);

	if (mWaitingForAnswer)
	{
		qCWarning(card_remote) << "RemoteDispatcher was closed while waiting for an answer:" << pCloseCode;

		mResponse.reset();
		mWaitingForAnswer = false;
		mWaitCondition.wakeOne();
	}
}


RemoteCard::RemoteCard(const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher, const QString& pReaderName)
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

	QObject::connect(mRemoteDispatcher.data(), &RemoteDispatcher::fireClosed, this, &RemoteCard::onDispatcherClosed, Qt::DirectConnection);
}


RemoteCard::~RemoteCard()
{
	mResponseAvailable.unlock();
}


CardReturnCode RemoteCard::connect()
{
	const QSharedPointer<IfdConnect> connectMsg(new IfdConnect(mReaderName));
	if (sendMessage(connectMsg, RemoteCardMessageType::IFDConnectResponse, 5000))
	{
		const QSharedPointer<const IfdConnectResponse> response = mResponse.dynamicCast<const IfdConnectResponse>();
		if (response)
		{
			if (!response->resultHasError())
			{
				mConnected = true;
				mSlotHandle = response->getSlotHandle();
				return CardReturnCode::OK;
			}
			qCWarning(card_remote) << response->getResultMinor();
		}
	}
	return CardReturnCode::COMMAND_FAILED;
}


CardReturnCode RemoteCard::disconnect()
{
	const QSharedPointer<IfdDisconnect> disconnectCmd(new IfdDisconnect(mSlotHandle));
	if (sendMessage(disconnectCmd, RemoteCardMessageType::IFDDisconnectResponse, 5000))
	{
		const QSharedPointer<const IfdDisconnectResponse> response = mResponse.dynamicCast<const IfdDisconnectResponse>();
		if (response)
		{
			if (!response->resultHasError())
			{
				mConnected = false;
				return CardReturnCode::OK;
			}
			qCWarning(card_remote) << response->getResultMinor();
		}
	}
	return CardReturnCode::COMMAND_FAILED;
}


bool RemoteCard::isConnected()
{
	return mConnected;
}


CardReturnCode RemoteCard::transmit(const CommandApdu& pCommand, ResponseApdu& pResponse)
{
	QSharedPointer<const IfdTransmit> transmitCmd(new IfdTransmit(mSlotHandle, pCommand.getBuffer()));
	if (sendMessage(transmitCmd, RemoteCardMessageType::IFDTransmitResponse, 5000))
	{
		const QSharedPointer<const IfdTransmitResponse> response = mResponse.dynamicCast<const IfdTransmitResponse>();
		if (response)
		{
			if (!response->resultHasError())
			{
				pResponse.setBuffer(response->getResponseApdu());
				return CardReturnCode::OK;
			}
			qCWarning(card_remote) << response->getResultMinor();
		}
	}
	return CardReturnCode::COMMAND_FAILED;
}


CardReturnCode RemoteCard::establishPaceChannel(PACE_PASSWORD_ID pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPACEChannelOutput& pChannelOutput, quint8 pTimeoutSeconds)
{
	EstablishPACEChannelBuilder builder;
	builder.setPasswordId(pPasswordId);
	builder.setChat(pChat);
	builder.setCertificateDescription(pCertificateDescription);
	const QByteArray inputData = builder.createCommandDataCcid().getBuffer();

	QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(mSlotHandle, inputData));
	if (sendMessage(message, RemoteCardMessageType::IFDEstablishPACEChannelResponse, pTimeoutSeconds * 1000))
	{
		const QSharedPointer<const IfdEstablishPaceChannelResponse> response = mResponse.dynamicCast<const IfdEstablishPaceChannelResponse>();
		if (response)
		{
			pChannelOutput.parseFromCcid(response->getOutputData(), pPasswordId);
			return pChannelOutput.getPaceReturnCode();
		}
	}

	return CardReturnCode::COMMAND_FAILED;
}


CardReturnCode RemoteCard::setEidPin(quint8 pTimeoutSeconds, ResponseApdu& pResponseApdu)
{
	PinModify pinModify(pTimeoutSeconds);
	const QByteArray inputData = pinModify.createCcidForRemote();

	QSharedPointer<const IfdModifyPin> message(new IfdModifyPin(mSlotHandle, inputData));
	if (sendMessage(message, RemoteCardMessageType::IFDModifyPINResponse, pTimeoutSeconds * 1000))
	{
		const QSharedPointer<const IfdModifyPinResponse> response = mResponse.dynamicCast<const IfdModifyPinResponse>();
		if (response)
		{
			PinModifyOutput output(response->getOutputData());
			pResponseApdu.setBuffer(output.getResponseApdu().getBuffer());
			if (response->resultHasError())
			{
				return response->getReturnCode();
			}
			else
			{
				return output.getReturnCode();
			}
		}

		const QSharedPointer<const IfdError> ifdError = mResponse.dynamicCast<const IfdError>();
		if (ifdError)
		{
			if (ifdError->getResultMinor() == QLatin1String("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownAPIFunction"))
			{
				return CardReturnCode::PROTOCOL_ERROR;
			}

			return CardReturnCode::UNKNOWN;
		}
	}

	return CardReturnCode::COMMAND_FAILED;
}
