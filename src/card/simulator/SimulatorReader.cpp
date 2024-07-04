/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SimulatorReader.h"

#include "AppSettings.h"
#include "Env.h"
#include "VolatileSettings.h"


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_simulator)


SimulatorReader::SimulatorReader()
	: ConnectableReader(ReaderManagerPluginType::SIMULATOR, QStringLiteral("Simulator"))
{
	setInfoBasicReader(Env::getSingleton<AppSettings>()->getSimulatorSettings().isBasicReader());
}


Card* SimulatorReader::getCard() const
{
	return mCard.data();
}


void SimulatorReader::insertCard(const QVariant& pData)
{
	if (getReaderInfo().hasCard())
	{
		qCDebug(card_simulator) << "Already inserted";
		return;
	}

	const auto& data = pData.toJsonObject();
	const auto& filesystem = data.isEmpty() ? SimulatorFileSystem() : SimulatorFileSystem(data);

	mCard.reset(new SimulatorCard(filesystem));
	fetchCardInfo();

	qCInfo(card_simulator) << "Card inserted:" << getReaderInfo().getCardInfo();
	Q_EMIT fireCardInserted(getReaderInfo());
}


void SimulatorReader::connectReader()
{
	qCDebug(card_simulator) << "targetDetected, type: Simulator";

	setInfoCardInfo(CardInfo(CardType::EID_CARD));
	shelveCard();

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		insertCard();
	}
#endif
}


void SimulatorReader::disconnectReader(const QString& pError)
{
	Q_UNUSED(pError)

	if (mCard)
	{
		qCDebug(card_simulator) << "targetLost, type: Simulator";

		mCard.reset();
		removeCardInfo();

		qCInfo(card_simulator) << "Card removed";
		Q_EMIT fireCardRemoved(getReaderInfo());
	}
}
