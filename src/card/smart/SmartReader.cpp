/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "SmartReader.h"

#include "SmartManager.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


SmartReader::SmartReader(const QString& pName)
	: ConnectableReader(ReaderManagerPluginType::SMART, pName)
	, mCard()
{
}


Card* SmartReader::getCard() const
{
	return mCard.data();
}


void SmartReader::connectReader()
{
	const auto& smartManager = SmartManager::get();
	if (const auto& status = smartManager->status(); status != EidStatus::PERSONALIZED)
	{
		qCDebug(card_smart) << "Skipped to connect the reader because the Smart-eID is not personalized:" << status;
		return;
	}

	qCDebug(card_smart) << "targetDetected, type: Smart";
	mCard.reset(new SmartCard());
	fetchCardInfo();
	shelveCard();
}


void SmartReader::disconnectReader(const QString& pError)
{
	Q_UNUSED(pError)

	auto readerInfo = getReaderInfo();
	const bool cardInserted = readerInfo.hasCard();
	const bool wasShelved = readerInfo.wasShelved();
	removeCardInfo();

	if (mCard)
	{
		qCDebug(card_smart) << "targetLost, type: Smart";
		mCard.reset();
	}

	if (cardInserted)
	{
		qCInfo(card_smart) << "Card removed";
		Q_EMIT fireCardRemoved(getReaderInfo());
		return;
	}

	if (wasShelved)
	{
		Q_EMIT fireReaderPropertiesUpdated(getReaderInfo());
	}
}
