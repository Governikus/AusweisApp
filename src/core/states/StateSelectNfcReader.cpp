/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ReaderManager.h"
#include "StateSelectNfcReader.h"
#include "context/ChangePinContext.h"

using namespace governikus;

StateSelectNfcReader::StateSelectNfcReader(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateSelectNfcReader::run()
{
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StateSelectNfcReader::onReaderDetected);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StateSelectNfcReader::onReaderDetected);
	mConnections += connect(getContext().data(), &WorkflowContext::fireAbortCardSelection, this, &StateSelectNfcReader::fireAbort);

	onReaderDetected();
}


void StateSelectNfcReader::onReaderDetected()
{
	auto readerInfos = ReaderManager::getInstance().getReaderInfos(ReaderManagerPlugInType::NFC);
	if (!readerInfos.isEmpty())
	{
		getContext()->setReaderName(readerInfos[0].getName());
		qDebug() << "Select first found Nfc reader" << readerInfos[0].getName();
		Q_EMIT fireSuccess();
		return;
	}

	getContext()->setReaderName(QString());
	qDebug() << "No Nfc reader detected";
}
