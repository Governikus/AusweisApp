/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "StateSelectPcscReader.h"

#include <QSharedPointer>
#include <QVector>

#include "ReaderManager.h"

using namespace governikus;

StateSelectPcscReader::StateSelectPcscReader(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateSelectPcscReader::run()
{
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StateSelectPcscReader::onReaderManagerSignal);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StateSelectPcscReader::onReaderManagerSignal);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StateSelectPcscReader::onReaderManagerSignal);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &StateSelectPcscReader::onReaderManagerSignal);
	mConnections += connect(getContext().data(), &WorkflowContext::fireAbortCardSelection, this, &StateSelectPcscReader::fireAbort);

	onReaderManagerSignal();
}


void StateSelectPcscReader::onReaderManagerSignal()
{
	QVector<ReaderInfo> readersWithNpa;
	const auto allReaders = ReaderManager::getInstance().getReaderInfos(ReaderManagerPlugInType::PCSC);
	for (const auto& readerInfo : allReaders)
	{
		if (readerInfo.getCardType() == CardType::EID_CARD)
		{
			readersWithNpa << readerInfo;
		}
	}

	if (readersWithNpa.size() == 1)
	{
		getContext()->setReaderName(readersWithNpa[0].getName());
		Q_EMIT fireSuccess();
	}
}
