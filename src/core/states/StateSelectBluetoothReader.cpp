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
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderDeviceError, this, &StateSelectBluetoothReader::onReaderDeviceError);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StateSelectBluetoothReader::onReaderDetected);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderConnected, this, &StateSelectBluetoothReader::onReaderConnected);

	startScan();
}


void StateSelectBluetoothReader::startScan()
{
	Q_ASSERT(getContext());

	const auto& pluginInfos = ReaderManager::getInstance().getPlugInInfos();
	for (const ReaderManagerPlugInInfo& pInfo : pluginInfos)
	{
		if (pInfo.getPlugInType() != ReaderManagerPlugInType::BLUETOOTH)
		{
			continue;
		}

		if (!pInfo.isEnabled())
		{
			qDebug() << "Skipping scan because bluetooth is disabled";
			return;
		}

		onReaderDetected();
		if (!getContext()->getReaderName().isEmpty())
		{
			qDebug() << "Skipping scan because there is already a paired device:" << getContext()->getReaderName();
			return;
		}

		qDebug() << "Start bluetooth scan";
		ReaderManager::getInstance().startScan();

		return;
	}
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
		setStatus(GlobalStatus::Code::Workflow_Reader_Became_Inaccessible);
		Q_EMIT fireError();
	}
	else if (readerInfo.isConnected())
	{
		qDebug() << "Bluetooth reader connected" << readerInfo.getName();
		Q_EMIT fireSuccess();
	}
}


void StateSelectBluetoothReader::onAbort()
{
	const auto& readerName = getContext()->getReaderName();
	if (!readerName.isEmpty())
	{
		ReaderInfo readerInfo = ReaderManager::getInstance().getReaderInfo(readerName);
		if (readerInfo.isValid() && readerInfo.isConnected())
		{
			ReaderManager::getInstance().disconnectReader(readerInfo.getName());
		}
	}
	Q_EMIT fireAbort();
}


void StateSelectBluetoothReader::onReaderDeviceError(DeviceError pDeviceError)
{
	if (pDeviceError != DeviceError::DEVICE_POWERED_OFF)
	{
		setStatus(DeviceErrorUtil::toGlobalStatus(pDeviceError));
		Q_EMIT fireError();
	}
}


void StateSelectBluetoothReader::onExit(QEvent* pEvent)
{
	AbstractGenericState::onExit(pEvent);
	ReaderManager::getInstance().stopScan();
}


void StateSelectBluetoothReader::onEntry(QEvent* pEvent)
{
	AbstractGenericState::onEntry(pEvent);
	mConnections += connect(getContext().data(), &WorkflowContext::fireAbortCardSelection, this, &StateSelectBluetoothReader::onAbort);
}
