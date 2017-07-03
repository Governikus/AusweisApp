/*!
 * BluetoothReaderManagerPlugIn.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "AndroidBluetoothAdapter.h"
#include "BluetoothDebug.h"
#include "BluetoothDeviceUtil.h"
#include "BluetoothReader.h"
#include "BluetoothReaderManagerPlugIn.h"
#include "BluetoothReaderManagerPlugIn_p.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(bluetooth)


using namespace governikus;


BluetoothReaderManagerPlugIn::BluetoothReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::BLUETOOTH)
	, d_ptr(new BluetoothReaderManagerPlugInPrivate(this))
	, mDeviceDiscoveryAgent()
	, mDiscoveredReadersInCurrentScan()
	, mInitializingDevices()
	, mReaders()
	, mScanInProgress(false)
{
	connect(&mDeviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothReaderManagerPlugIn::onDeviceDiscovered);
	connect(&mDeviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothReaderManagerPlugIn::onDeviceDiscoveryFinished);
	connect(&mDeviceDiscoveryAgent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error), this, &BluetoothReaderManagerPlugIn::onDeviceDiscoveryError);
	connect(&mDeviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BluetoothReaderManagerPlugIn::onDeviceDiscoveryCanceled);
}


void BluetoothReaderManagerPlugIn::init()
{
	ReaderManagerPlugIn::init();
	Q_D(BluetoothReaderManagerPlugIn);
	d->init();
}


QList<Reader*> BluetoothReaderManagerPlugIn::getReader() const
{
	QList<Reader*> readers;
	readers.reserve(mReaders.size());
	for (auto bluetoothReader : mReaders)
	{
		readers += bluetoothReader;
	}
	return readers;
}


void BluetoothReaderManagerPlugIn::startScan()
{
	if (mDeviceDiscoveryAgent.isActive())
	{
		qCWarning(bluetooth) << "Bluetooth device discovery already running";
		return;
	}

	Q_D(BluetoothReaderManagerPlugIn);
	d->onScanStart();

	qCDebug(bluetooth) << "Handle already paired devices";
	d->handlePairedDevices();

	qCDebug(bluetooth) << "Starting Bluetooth device discovery";
	mScanInProgress = true;
	mDeviceDiscoveryAgent.start();
}


void BluetoothReaderManagerPlugIn::setBluetoothStatus(bool pEnabled)
{
	setReaderInfoEnabled(pEnabled);
	if (!pEnabled)
	{
		while (!mReaders.isEmpty())
		{
			onRemoveReader(mReaders.firstKey());
		}
	}
}


void BluetoothReaderManagerPlugIn::stopScan()
{
	if (!mDeviceDiscoveryAgent.isActive())
	{
		qCWarning(bluetooth) << "Bluetooth device discovery not running";
		return;
	}

	qCDebug(bluetooth) << "Stopping Bluetooth device discovery";
	mScanInProgress = false;
	mDeviceDiscoveryAgent.stop();
}


void BluetoothReaderManagerPlugIn::onDeviceDiscovered(const QBluetoothDeviceInfo& pInfo)
{
	qCDebug(bluetooth) << "Bluetooth device discovered" << pInfo;

	QString deviceId = BluetoothDeviceUtil::getDeviceId(pInfo);
	if (mReaders.contains(deviceId))
	{
		mDiscoveredReadersInCurrentScan.append(deviceId);
		return;
	}
	if (mInitializingDevices.contains(deviceId))
	{
		qCDebug(bluetooth) << "Device is already determined, just update the device info";
		auto knownDevice = mInitializingDevices.value(deviceId);
		knownDevice->setDeviceInfo(pInfo);
		return;
	}

	QSharedPointer<CyberJackWaveDevice> newDevice(new CyberJackWaveDevice(pInfo));
	mInitializingDevices.insert(deviceId, newDevice);
	connect(newDevice.data(), &CyberJackWaveDevice::fireInitialized, this, &BluetoothReaderManagerPlugIn::onDeviceInitialized);
	newDevice->initialize();
}


void BluetoothReaderManagerPlugIn::onDeviceInitialized(const QBluetoothDeviceInfo& pInfo)
{
	QString deviceId = BluetoothDeviceUtil::getDeviceId(pInfo);
	if (auto device = mInitializingDevices.value(deviceId))
	{
		if (device->isValid())
		{
			if (mReaders.contains(deviceId))
			{
				qCDebug(bluetooth) << "Device is already determined as reader, skip further processing";
				mDiscoveredReadersInCurrentScan.append(deviceId);
			}
			else
			{
				BluetoothReader* reader = new BluetoothReader(device);
				qCDebug(bluetooth) << "Device is successfully initialized, create reader" << reader->getName();

				connect(reader, &BluetoothReader::fireReaderConnected, this, &ReaderManagerPlugIn::fireReaderConnected);
				connect(reader, &Reader::fireCardInserted, this, &ReaderManagerPlugIn::fireCardInserted);
				connect(reader, &Reader::fireCardRemoved, this, &ReaderManagerPlugIn::fireCardRemoved);
				connect(reader, &Reader::fireCardRetryCounterChanged, this, &ReaderManagerPlugIn::fireCardRetryCounterChanged);

				mDiscoveredReadersInCurrentScan.append(deviceId);
				mReaders.insert(deviceId, reader);
				connect(device.data(), &CyberJackWaveDevice::fireDisconnected, this, &BluetoothReaderManagerPlugIn::onDeviceDisconnected);
			}
		}
		disconnect(device.data(), &CyberJackWaveDevice::fireInitialized, this, &BluetoothReaderManagerPlugIn::onDeviceInitialized);
		device->disconnectFromDevice();
	}
}


void BluetoothReaderManagerPlugIn::onDeviceDisconnected(const QBluetoothDeviceInfo& pInfo)
{
	QString deviceId = BluetoothDeviceUtil::getDeviceId(pInfo);
	if (auto device = mInitializingDevices.value(deviceId))
	{
		disconnect(device.data(), &CyberJackWaveDevice::fireDisconnected, this, &BluetoothReaderManagerPlugIn::onDeviceDisconnected);
		if (auto reader = mReaders.value(deviceId))
		{
			qCDebug(bluetooth) << "Device is disconnected, publish reader" << reader->getName();
			Q_EMIT fireReaderAdded(reader->getName());
		}
	}
}


void BluetoothReaderManagerPlugIn::onDeviceDiscoveryFinished()
{
	if (mScanInProgress)
	{
		mDeviceDiscoveryAgent.start();
		return;
	}
	qCDebug(bluetooth) << "Stopped Bluetooth device discovery";
}


void BluetoothReaderManagerPlugIn::onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error pError)
{
	qCCritical(bluetooth) << "Error on Bluetooth device discovery" << pError;
}


void BluetoothReaderManagerPlugIn::onDeviceDiscoveryCanceled()
{
	qCDebug(bluetooth) << "Bluetooth device discovery canceled";

	Q_D(BluetoothReaderManagerPlugIn);
	d->onDeviceDiscoveryCanceled();

	mInitializingDevices.clear();
	mDiscoveredReadersInCurrentScan.clear();
}


void BluetoothReaderManagerPlugIn::onRemoveReader(const QString& pDeviceId)
{
	if (auto reader = mReaders.take(pDeviceId))
	{
		Q_EMIT fireReaderRemoved(reader->getName());
		delete reader;
	}
}
