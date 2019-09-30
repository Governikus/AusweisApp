/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnectionWorker.h"
#include "IosReader.h"

#include <QDateTime>
#include <QLoggingCategory>
#include <QSignalBlocker>
#include <QTimer>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


Reader::CardEvent IosReader::updateCard()
{
	if (mCard && !mCard->isValid())
	{
		removeCard();
		mDelegate.startSession();
		return CardEvent::CARD_REMOVED;
	}

	return CardEvent::NONE;
}


IosReader::IosReader()
	: ConnectableReader(ReaderManagerPlugInType::NFC, QStringLiteral("NFC"))
	, mDelegate()
	, mCard()
	, mConnected(false)
	, mLastRestart(0)
{
	mReaderInfo.setBasicReader(true);
	mReaderInfo.setConnected(true);

	connect(&mDelegate, &IosReaderDelegate::fireDiscoveredTag, this, &IosReader::onDiscoveredTag);
	connect(&mDelegate, &IosReaderDelegate::fireDidInvalidateWithError, this, &IosReader::onDidInvalidateWithError);

	mTimerId = startTimer(500);
}


void IosReader::removeCard()
{
	Q_ASSERT(mCard);
	mCard->invalidateTarget();
	mCard.reset();
	mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
}


void IosReader::stopSession(const QString& pError)
{
	qCDebug(card_nfc) << pError;

	if (pError.isEmpty())
	{
		//: INFO IOS The current session was stopped without errors.
		mDelegate.stopSession(tr("Scanning process has been finished successfully."));
	}
	else
	{
		mDelegate.stopSession(pError, true);
	}

	if (mCard)
	{
		removeCard();
		Q_EMIT fireCardRemoved(getName());
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
	mDelegate.startSession();
}


void IosReader::disconnectReader(const QString& pError)
{
	stopSession(pError);
	mConnected = false;
}


void IosReader::onDiscoveredTag(IosCard* pCard)
{
	mCard.reset(pCard);
	connect(pCard, &IosCard::fireTransmitFailed, this, &IosReader::onTransmitFailed);
	QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
	CardInfoFactory::create(cardConnection, mReaderInfo);
	Q_EMIT fireCardInserted(getName());
}


void IosReader::onDidInvalidateWithError(const QString& pError, bool pDoRestart)
{
	stopSession(pError);

	if (pDoRestart && mConnected)
	{
		const qint64 now = QDateTime::currentSecsSinceEpoch();
		if (now - mLastRestart <= 1) // Don't restart more than once per second to avoid spamming the log
		{
			using namespace std::chrono_literals;
			QTimer::singleShot(1s, this, [this](){
						if (mConnected)
						{
							mDelegate.startSession();
						}
					});
		}
		else
		{
			mDelegate.startSession();
		}
		mLastRestart = now;
		return;
	}

	mConnected = false;
	Q_EMIT fireReaderDisconnected();
}


void IosReader::onConnectFailed()
{
	//: ERROR IOS The connection to the card could not be established.
	stopSession(tr("The connection could not be established. The process was aborted."));
	mConnected = false;
	Q_EMIT fireReaderDisconnected();
}


void IosReader::onTransmitFailed()
{
	//: ERROR IOS The card was removed during the communication.
	stopSession(tr("The connection to the ID card has been lost. The process was aborted."));
	mConnected = false;
	Q_EMIT fireReaderDisconnected();
}
