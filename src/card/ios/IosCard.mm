/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "IosCard.h"

#include "IosCardPointer.h"

#include <QLoggingCategory>

#import <CoreNFC/NFCISO7816Tag.h>
#import <CoreNFC/NFCReaderSession.h>
#import <CoreNFC/NFCTagReaderSession.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


IosCard::IosCard(IosCardPointer* const pCard)
	: Card()
	, mCard(pCard)
	, mConnected(false)
	, mWaitCondition()
	, mCallbackDone()
{
	qCDebug(card_nfc) << "Card created";

	[mCard->mNfcTag retain];
	mCallbackDone.lock();
}


IosCard::~IosCard()
{
	[mCard->mNfcTag release];
	delete mCard;
}


bool IosCard::isValid() const
{
	return mCard->mNfcTag && (!mConnected || [mCard->mNfcTag isAvailable]);
}


bool IosCard::invalidateTarget()
{
	if (isValid())
	{
		[mCard->mNfcTag release];
		mCard->mNfcTag = nil;
		return true;
	}

	return false;
}


CardReturnCode IosCard::connect()
{
	if (!isValid())
	{
		qCWarning(card_nfc) << "NearFieldTarget is no longer valid";
		return CardReturnCode::COMMAND_FAILED;
	}

	if (isConnected())
	{
		qCCritical(card_nfc) << "Card is already connected";
		return CardReturnCode::OK;
	}

	NFCTagReaderSession* session = [mCard->mNfcTag session];
	[session connectToTag: mCard->mNfcTag completionHandler: ^(NSError* error){
		if (error != nil)
		{
			qCDebug(card_nfc) << "Error during connect:" << error;
		}
		else
		{
			mConnected = true;
		}

		mWaitCondition.wakeOne();
	}];

	mWaitCondition.wait(&mCallbackDone, 500);
	if (!mConnected)
	{
		Q_EMIT fireTransmitFailed();
		return CardReturnCode::COMMAND_FAILED;
	}

	return CardReturnCode::OK;
}


CardReturnCode IosCard::disconnect()
{
	if (!isValid())
	{
		qCWarning(card_nfc) << "NearFieldTarget is no longer valid";
		return CardReturnCode::COMMAND_FAILED;
	}

	if (!isConnected())
	{
		qCCritical(card_nfc) << "Card is already disconnected";
		return CardReturnCode::COMMAND_FAILED;
	}

	mConnected = false;
	return CardReturnCode::OK;
}


bool IosCard::isConnected()
{
	return mConnected;
}


void IosCard::setProgressMessage(const QString& pMessage)
{
	NFCTagReaderSession* session = [mCard->mNfcTag session];
	[session setAlertMessage: pMessage.toNSString()];
}


CardReturnCode IosCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes)
{
	if (!isValid())
	{
		qCWarning(card_nfc) << "NearFieldTarget is no longer valid";
		return CardReturnCode::COMMAND_FAILED;
	}

	qCDebug(card_nfc) << "Transmit command APDU:" << pCmd.getBuffer().toHex();

	const auto resultBuffer = QSharedPointer<QByteArray>::create(); // Don't use this inside of the Block
	const QWeakPointer<QByteArray> weakBuffer = resultBuffer;

	Q_ASSERT([mCard->mNfcTag conformsToProtocol:@protocol(NFCISO7816Tag)]);
	const auto tag = static_cast<id<NFCISO7816Tag> >(mCard->mNfcTag);
	auto* apdu = [[NFCISO7816APDU alloc] initWithData: pCmd.getBuffer().toNSData()];
	[tag sendCommandAPDU: apdu completionHandler: ^(NSData* responseData, uint8_t sw1, uint8_t sw2, NSError* error){
	    // By referencing weakBuffer here, it will be copied into the Block. If the handler outlives the caller, resultBuffer won't exist anymore.
		if (const auto recvBuffer = weakBuffer.lock())
		{
			if (error == nil)
			{
				*recvBuffer = QByteArray::fromNSData(responseData);
				*recvBuffer += static_cast<char>(sw1);
				*recvBuffer += static_cast<char>(sw2);
				qCDebug(card_nfc) << "Transmit response APDU:" << recvBuffer->toHex();
			}
			else
			{
				qCDebug(card_nfc) << "Error during transmit:" << error;
			}

			mWaitCondition.wakeOne();
		}
		else
		{
			qCDebug(card_nfc) << "Caller doesn't exist anymore.";
		}
	}];

	mWaitCondition.wait(&mCallbackDone, 500);
	if (resultBuffer->isEmpty())
	{
		Q_EMIT fireTransmitFailed();
		return CardReturnCode::COMMAND_FAILED;
	}

	pRes.setBuffer(std::move(*resultBuffer));
	return CardReturnCode::OK;
}
