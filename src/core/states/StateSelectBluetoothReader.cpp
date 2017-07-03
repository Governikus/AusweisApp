/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ReaderManager.h"
#include "StateSelectBluetoothReader.h"

#include <QTimer>

using namespace governikus;

StateSelectBluetoothReader::StateSelectBluetoothReader(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateSelectBluetoothReader::run()
{
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StateSelectBluetoothReader::onReaderDetected);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StateSelectBluetoothReader::onReaderDetected);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderConnected, this, &StateSelectBluetoothReader::onReaderConnected);
	mConnections += connect(getContext().data(), &WorkflowContext::fireAbortCardSelection, this, &StateSelectBluetoothReader::fireAbort);

	ReaderManager::getInstance().startScan();
	onReaderDetected();
}


void StateSelectBluetoothReader::onReaderDetected()
{
	auto readerInfos = ReaderManager::getInstance().getReaderInfos(ReaderManagerPlugInType::BLUETOOTH);
	if (!readerInfos.isEmpty())
	{
		getContext()->setReaderName(readerInfos[0].getName());
		qDebug() << "Select first found Bluetooth reader" << readerInfos[0].getName();
		ReaderManager::getInstance().stopScan();

		/*
		 * Workaround for pairing problem on Android:
		 *
		 * The reader detection process performs a BT connect, determines the services and characteristics
		 * and disconnects afterwards. When selecting a reader we connect again.
		 * This causes some timing issue. Other developers have similar problems, e.g. see
		 * https://github.com/NordicSemiconductor/Android-DFU-Library/issues/1#issuecomment-156790789
		 *
		 * This lead to setting a delay of 1000 msecs.
		 */
		QTimer::singleShot(1000, [ = ] {ReaderManager::getInstance().connectReader(readerInfos[0].getName());
				});
	}
}


void StateSelectBluetoothReader::onReaderConnected()
{
	ReaderInfo readerInfo = ReaderManager::getInstance().getReaderInfo(getContext()->getReaderName());
	if (!readerInfo.isValid())
	{
		setResult(Result::createCommunicationError(tr("The selected card reader cannot be accessed anymore.")));
		Q_EMIT fireError();
	}
	else if (readerInfo.isConnected())
	{
		qDebug() << "Bluetooth reader connected" << readerInfo.getName();
		Q_EMIT fireSuccess();
	}
}
