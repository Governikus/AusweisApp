/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SimulatorReader.h"

#include "Env.h"
#include "VolatileSettings.h"

using namespace governikus;


SimulatorReader::SimulatorReader()
	: ConnectableReader(ReaderManagerPlugInType::SIMULATOR, QStringLiteral("Simulator"))
{
	setInfoBasicReader(false);
}


Card* SimulatorReader::getCard() const
{
	return mCard.data();
}


void SimulatorReader::insertCard(const QVariant& pData)
{
	if (getReaderInfo().hasCard())
	{
		qCDebug(card) << "Already inserted";
		return;
	}

	const auto& data = pData.toJsonObject();
	const auto& filesystem = data.isEmpty() ? SimulatorFileSystem() : SimulatorFileSystem(data);

	mCard.reset(new SimulatorCard(filesystem));
	QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
	fetchCardInfo(cardConnection);

	Q_EMIT fireCardInserted(getReaderInfo());
}


void SimulatorReader::connectReader()
{
	qCDebug(card) << "targetDetected, type: Simulator";

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
		qCDebug(card) << "targetLost, type: Simulator";

		mCard.reset();
		removeCardInfo();
		Q_EMIT fireCardRemoved(getReaderInfo());
	}
}
