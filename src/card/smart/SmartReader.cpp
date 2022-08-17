/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SmartReader.h"

#include "SmartManager.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


SmartReader::SmartReader()
	: ConnectableReader(ReaderManagerPlugInType::SMART, QStringLiteral("Smart"))
	, mCard()
{
}


Card* SmartReader::getCard() const
{
	return mCard.data();
}


void SmartReader::connectReader()
{
	switch (const auto& status = SmartManager::get()->status())
	{
		case EidStatus::PERSONALIZED:
		{
			qCDebug(card_smart) << "targetDetected, type: Smart";

			mCard.reset(new SmartCard());
			QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
			fetchCardInfo(cardConnection);
			shelveCard();
			break;
		}

		default:
			qCDebug(card_smart) << "Skipped to connect the reader because the Smart-eID is not personalized:" << status;
	}
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
		Q_EMIT fireCardRemoved(getReaderInfo());
		return;
	}

	if (wasShelved)
	{
		Q_EMIT fireReaderPropertiesUpdated(getReaderInfo());
	}
}
