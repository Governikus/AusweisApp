/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "IosReader.h"

#include "CardConnectionWorker.h"
#include "IosCardPointer.h"
#include "IosReaderDelegate.h"
#include "VolatileSettings.h"

#include <QDateTime>
#include <QLoggingCategory>
#include <QTimer>

#import <CoreNFC/NFCISO7816Tag.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


Reader::CardEvent IosReader::updateCard()
{
	if (mCard && !mCard->isValid())
	{
		onDidInvalidateWithError(true);
	}

	return CardEvent::NONE;
}


IosReader::IosReader()
	: ConnectableReader(ReaderManagerPlugInType::NFC, QStringLiteral("NFC"))
	, mCard()
	, mConnected(false)
	, mIsRestarting(false)
{
	mReaderInfo.setBasicReader(true);
	mReaderInfo.setConnected(true);

	if (@available(iOS 13, *))
	{
		mDelegate = [[IosReaderDelegate alloc] initWithListener:this];
	}

	connect(this, &IosReader::fireTagDiscovered, this, &IosReader::onTagDiscovered, Qt::QueuedConnection);
	connect(this, &IosReader::fireDidInvalidateWithError, this, &IosReader::onDidInvalidateWithError, Qt::QueuedConnection);

	mTimerId = startTimer(500);
}


void IosReader::removeCard()
{
	Q_ASSERT(mCard);
	mCard->invalidateTarget();
	mCard.reset();
	mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
}


void IosReader::startSession()
{
	if (@available(iOS 13, *))
	{
		if (mConnected)
		{
			[mDelegate alertMessage: Env::getSingleton<VolatileSettings>()->isUsedAsSDK()
			//: INFO IOS The ID card may be inserted, the authentication process may be started.
			? Env::getSingleton<VolatileSettings>()->getMessages().getSessionStarted() : tr("Please place your ID card on the top of the device's back side.")];
			[mDelegate startSession];
		}
	}
}


void IosReader::stopSession(const QString& pError)
{
	if (!pError.isNull())
	{
		qCDebug(card_nfc) << "stopSession with error:" << pError;
	}

	if (mCard)
	{
		removeCard();
		Q_EMIT fireCardRemoved(mReaderInfo);
	}

	if (@available(iOS 13, *))
	{
		const bool isSdk = Env::getSingleton<VolatileSettings>()->isUsedAsSDK();
		if (pError.isNull())
		{
			//: INFO IOS The current session was stopped without errors.
			[mDelegate alertMessage: isSdk ? Env::getSingleton<VolatileSettings>()->getMessages().getSessionSucceeded() : tr("Scanning process has been finished successfully.")];
			[mDelegate stopSession: QString()];
		}
		else
		{
			[mDelegate stopSession: isSdk ? Env::getSingleton<VolatileSettings>()->getMessages().getSessionFailed() : pError];
		}
	}
}


IosReader::~IosReader()
{
}


Card* IosReader::getCard() const
{
	if (mCard && mCard->isValid())
	{
		return mCard.data();
	}

	return nullptr;
}


void IosReader::connectReader()
{
	mConnected = true;
	startSession();
}


void IosReader::disconnectReader(const QString& pError)
{
	stopSession(pError);
	mConnected = false;
}


void IosReader::onTagDiscovered(IosCardPointer* pCard)
{
	if (@available(iOS 13, *))
	{
		id<NFCISO7816Tag> iso7816Tag = pCard->mNfcTag.asNFCISO7816Tag;
		if (iso7816Tag)
		{
			if (iso7816Tag.historicalBytes != nil && iso7816Tag.applicationData == nil)
			{
				qCDebug(card_nfc) << "targetDetected, type: QNearFieldTarget::NfcTagType4A";
			}

			if (iso7816Tag.historicalBytes == nil && iso7816Tag.applicationData != nil)
			{
				qCDebug(card_nfc) << "targetDetected, type: QNearFieldTarget::NfcTagType4B";
			}
		}
	}

	mCard.reset(new IosCard(pCard));
	QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
	if (CardInfoFactory::create(cardConnection, mReaderInfo) && mCard)
	{
		//: INFO IOS Feedback when a new ID card has been detected
		mCard->setProgressMessage(tr("ID card detected. Please do not move the device!"));
	}
	Q_EMIT fireCardInserted(mReaderInfo);
}


void IosReader::onDidInvalidateWithError(bool pDoRestart)
{
	if (mCard)
	{
		removeCard();
		Q_EMIT fireCardRemoved(mReaderInfo);
	}

	if (mConnected && pDoRestart)
	{
		if (!mIsRestarting)
		{
			mIsRestarting = true;

			using namespace std::chrono_literals;
			QTimer::singleShot(2s, this, [this](){
					mIsRestarting = false;
					startSession();
				});
		}
		return;
	}

	mConnected = false;
	Q_EMIT fireReaderDisconnected();
}
