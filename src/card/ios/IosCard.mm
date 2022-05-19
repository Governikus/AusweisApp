/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "IosCard.h"

#include "IosCardPointer.h"

#include <QElapsedTimer>
#include <QLoggingCategory>
#include <QSemaphore>
#include <QSharedPointer>

#import <CoreNFC/NFCISO7816Tag.h>
#import <CoreNFC/NFCReaderSession.h>
#import <CoreNFC/NFCTagReaderSession.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


IosCard::IosCard(IosCardPointer* const pCard)
	: Card()
	, mCard(pCard)
	, mConnected(false)
{
	qCDebug(card_nfc) << "Card created";
}


IosCard::~IosCard()
{
	delete mCard;
}


bool IosCard::isValid() const
{
	if (@available(iOS 13, *))
	{
		return mCard->mNfcTag;
	}

	return false;
}


void IosCard::invalidateTarget()
{
	mCard->mNfcTag = nil;
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

	const auto resultValue = QSharedPointer<bool>::create(false); // Don't use this inside of the Block
	const QWeakPointer<bool> weakValue = resultValue;

	if (@available(iOS 13, *))
	{
		const auto locker = QSharedPointer<QSemaphore>::create();

		NFCTagReaderSession* session = mCard->mNfcTag.session;
		[session connectToTag: mCard->mNfcTag completionHandler: ^(NSError* error){
		    // By referencing weakValue here, it will be copied into the Block. If the handler outlives the caller, resultValue won't exist anymore.
			const auto recvValue = weakValue.lock();
			if (!recvValue)
			{
				qCDebug(card_nfc) << "Caller doesn't exist anymore.";
				return;
			}

			if (error == nil)
			{
				*recvValue = true;
			}
			else
			{
				qCDebug(card_nfc) << "Error during connect:" << error;
			}

			locker->release();
		}];

		locker->tryAcquire(1, 2000);
	}

	if (!*resultValue)
	{
		invalidateTarget();
		return CardReturnCode::COMMAND_FAILED;
	}

	mConnected = true;
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


void IosCard::setProgressMessage(const QString& pMessage, int pProgress)
{
	if (@available(iOS 13, *))
	{
		QString message = generateProgressMessage(pMessage, pProgress);
		NFCTagReaderSession* session = mCard->mNfcTag.session;
		session.alertMessage = message.toNSString();
	}
}


ResponseApduResult IosCard::transmit(const CommandApdu& pCmd)
{
	if (!isValid())
	{
		qCWarning(card_nfc) << "NearFieldTarget is no longer valid";
		return {CardReturnCode::COMMAND_FAILED};
	}

	qCDebug(card_nfc) << "Transmit command APDU:" << pCmd.getBuffer().toHex();

	const auto resultBuffer = QSharedPointer<QByteArray>::create(); // Don't use this inside of the Block
	const QWeakPointer<QByteArray> weakBuffer = resultBuffer;

	if (@available(iOS 13, *))
	{
		const auto locker = QSharedPointer<QSemaphore>::create();

		Q_ASSERT([mCard->mNfcTag conformsToProtocol:@protocol(NFCISO7816Tag)]);
		const auto tag = static_cast<id<NFCISO7816Tag>>(mCard->mNfcTag);
		auto* apdu = [[NFCISO7816APDU alloc] initWithData: pCmd.getBuffer().toNSData()];
		[tag sendCommandAPDU: apdu completionHandler: ^(NSData* responseData, uint8_t sw1, uint8_t sw2, NSError* error){
		    // By referencing weakBuffer here, it will be copied into the Block. If the handler outlives the caller, resultBuffer won't exist anymore.
			const auto recvBuffer = weakBuffer.lock();
			if (!recvBuffer)
			{
				qCDebug(card_nfc) << "Caller doesn't exist anymore.";
				return;
			}

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

			locker->release();
		}];

		locker->tryAcquire(1, 2000);
	}

	if (resultBuffer->isEmpty())
	{
		invalidateTarget();
		return {CardReturnCode::COMMAND_FAILED};
	}

	return {CardReturnCode::OK, ResponseApdu(*resultBuffer)};
}
