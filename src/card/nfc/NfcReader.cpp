/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnectionWorker.h"
#include "NfcReader.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


Reader::CardEvent NfcReader::updateCard()
{
	Q_EMIT fireNfcAdapterStateChanged(mNfManager.isAvailable());

	return CardEvent::NONE;
}


void NfcReader::targetDetected(QNearFieldTarget* pTarget)
{
	if (pTarget == nullptr)
	{
		return;
	}
	qCDebug(card_nfc) << "targetDetected, type:" << pTarget->type();

	if (!(pTarget->accessMethods() & QNearFieldTarget::TagTypeSpecificAccess))
	{
		qCDebug(card_nfc) << "The target does not provide commands";
		return;
	}

	int length = pTarget->maxCommandLength();
	mReaderInfo.setMaxApduLength(length);
	if (!mReaderInfo.sufficientApduLength())
	{
		Q_EMIT fireReaderPropertiesUpdated(getName());
		qCDebug(card_nfc) << "ExtendedLengthApduSupport missing. MaxTransceiveLength:" << length;
	}

	mCard.reset(new NfcCard(pTarget));
	QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
	CardInfoFactory::create(cardConnection, mReaderInfo);
	Q_EMIT fireCardInserted(getName());
}


void NfcReader::targetLost(QNearFieldTarget* pTarget)
{
	qCDebug(card_nfc) << "targetLost";
	if (pTarget != nullptr && mCard && mCard->invalidateTarget(pTarget))
	{
		mCard.reset();
		mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
		Q_EMIT fireCardRemoved(getName());
	}
}


NfcReader::NfcReader()
	: Reader(ReaderManagerPlugInType::NFC, QStringLiteral("NFC"))
	, mNfManager()
{
	mReaderInfo.setBasicReader(true);
	mReaderInfo.setConnected(true);

	connect(&mNfManager, &QNearFieldManager::targetDetected, this, &NfcReader::targetDetected);
	connect(&mNfManager, &QNearFieldManager::targetLost, this, &NfcReader::targetLost);
	mNfManager.startTargetDetection();

	mTimerId = startTimer(500);
}


NfcReader::~NfcReader()
{
	mNfManager.stopTargetDetection();
}


Card* NfcReader::getCard() const
{
	if (mCard && mCard->isValid())
	{
		return mCard.data();
	}

	return nullptr;
}
