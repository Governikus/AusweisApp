/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "IfdReader.h"

#include "CardConnectionWorker.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_remote)


IfdReader::IfdReader(ReaderManagerPlugInType pPlugInType, const QString& pReaderName, const QSharedPointer<IfdDispatcherClient>& pDispatcher, const IfdStatus& pIfdStatus)
	: Reader(pPlugInType, pReaderName)
	, mCard(nullptr)
	, mDispatcher(pDispatcher)
{
	setInfoBasicReader(!pIfdStatus.hasPinPad());

	updateStatus(pIfdStatus);
}


IfdReader::~IfdReader()
{
	mCard.reset();
}


Card* IfdReader::getCard() const
{
	return mCard.data();
}


void IfdReader::updateStatus(const IfdStatus& pIfdStatus)
{

	if (getReaderInfo().getMaxApduLength() != pIfdStatus.getMaxApduLength())
	{
		setInfoMaxApduLength(pIfdStatus.getMaxApduLength());
		Q_EMIT fireReaderPropertiesUpdated(getReaderInfo());

		if (getReaderInfo().insufficientApduLength())
		{
			qCDebug(card_remote) << "ExtendedLengthApduSupport missing. maxAPDULength:" << getReaderInfo().getMaxApduLength();
		}
	}

	if (mCard)
	{
		if (pIfdStatus.getCardAvailable())
		{
			qCDebug(card_remote) << "Card is already known";
			return;
		}

		const bool cardInserted = getReaderInfo().getCardInfo().getCardType() != CardType::NONE;

		mCard.reset();
		removeCardInfo();

		if (cardInserted)
		{
			qCDebug(card_remote) << "Card removed";
			Q_EMIT fireCardRemoved(getReaderInfo());
		}
		else
		{
			qCDebug(card_remote) << "Card removed (was not inserted)";
			Q_EMIT fireReaderPropertiesUpdated(getReaderInfo());
		}

		return;
	}

	if (pIfdStatus.getCardAvailable())
	{
		mCard.reset(new IfdCard(mDispatcher, getName()));
		QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
		fetchCardInfo(cardConnection);

		if (getReaderInfo().getCardType() == CardType::SMART_EID)
		{
			qCDebug(card_remote) << "Card received but not inserted";
			shelveCard();
			return;
		}

		qCDebug(card_remote) << "Card inserted";
		Q_EMIT fireCardInserted(getReaderInfo());
	}
}
