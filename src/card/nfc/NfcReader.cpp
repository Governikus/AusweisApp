/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "NfcReader.h"

#include "CardConnectionWorker.h"
#include "NfcBridge.h"

#include <QLoggingCategory>
#include <QSignalBlocker>

Q_DECLARE_LOGGING_CATEGORY(card_nfc)

using namespace governikus;

NfcReader::NfcReader()
	: Reader(ReaderManagerPlugInType::NFC, QStringLiteral("NFC"), ReaderType::UNKNOWN)
{
	mReaderInfo.setBasicReader(true);
	mReaderInfo.setConnected(true);

	mTimerId = startTimer(500);
}


NfcReader::~NfcReader()
{
}


Card* NfcReader::getCard() const
{
	return mCard.data();
}


Reader::CardEvent NfcReader::updateCard()
{
#ifdef Q_OS_ANDROID

	const ExtendedLengthApduSupportCode& code = NfcBridge::getInstance().getExtendedLengthApduSupportStatus();
	if (mReaderInfo.getExtendedLengthApduSupportCode() != code)
	{
		mReaderInfo.setExtendedLengthApduSupportCode(code);
		Q_EMIT fireReaderPropertiesUpdated();
	}

	if (mCard.isNull() && NfcBridge::getInstance().getCardStatus() == NfcCardCode::NEW_CARD)
	{
		mCard.reset(new NfcCard());
		connect(mCard.data(), &NfcCard::fireCardRemoved, this, &NfcReader::onCardRemoved);
		QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
		CardInfoFactory::create(cardConnection, mReaderInfo);
		const QSignalBlocker blocker(this);
		updateRetryCounter(cardConnection);
		return CardEvent::CARD_INSERTED;
	}

	if (!mCard.isNull())
	{
		QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
		static CommandApdu selectMfCommand = SelectBuilder(FileRef::masterFile()).build();
		static ResponseApdu response;
		const QSignalBlocker blockFireCardRemoved(mCard.data());
		if (!cardConnection.isNull() && cardConnection->transmit(selectMfCommand, response) != ReturnCode::OK)
		{
			qCDebug(card_nfc) << "Card removed";
			mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
			mCard.reset();
			return CardEvent::CARD_REMOVED;
		}
	}

#endif

	return CardEvent::NONE;
}


void NfcReader::onCardRemoved()
{
	qCDebug(card_nfc) << "Card removed";
	mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
	mCard.reset();
	Q_EMIT fireCardRemoved(getName());

}
