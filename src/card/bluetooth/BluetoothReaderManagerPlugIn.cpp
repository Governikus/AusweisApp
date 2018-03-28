/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothReaderManagerPlugIn.h"

#include "AndroidBluetoothAdapter.h"
#include "BluetoothDebug.h"
#include "BluetoothDeviceUtil.h"
#include "BluetoothReader.h"
#include "BluetoothReaderManagerPlugIn_p.h"
#include "DeviceError.h"
#include "Initializer.h"
#include "ScopeGuard.h"

#include <QLoggingCategory>
#include <QTimer>

Q_DECLARE_LOGGING_CATEGORY(bluetooth)

using namespace governikus;


BluetoothReaderManagerPlugIn::BluetoothReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::BLUETOOTH)
	, d_ptr(new BluetoothReaderManagerPlugInPrivate(this))
	, mDeviceDiscoveryAgent()
	, mInitializingDevices()
	, mReaders()
	, mReadersDiscoveredInCurrentScan()
	, mTimerIdDiscoverPairedDevices(0)
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

	qCDebug(bluetooth) << "Handle already paired devices";
	d->handlePairedDevices();
	mTimerIdDiscoverPairedDevices = startTimer(1000);
}


QList<Reader*> BluetoothReaderManagerPlugIn::getReaders() const
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

	if (mReaders.size() < 1)
	{
		qCDebug(bluetooth) << "Starting Bluetooth device discovery";
		mScanInProgress = true;
		mDeviceDiscoveryAgent.start();
	}
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
	if (mDeviceDiscoveryAgent.isActive())
	{
		qCDebug(bluetooth) << "Stopping Bluetooth device discovery";
		mDeviceDiscoveryAgent.stop();
	}
	else
	{
		qCWarning(bluetooth) << "Bluetooth device discovery not running";
	}

	mScanInProgress = false;
}


void BluetoothReaderManagerPlugIn::onConnectToKnownReadersChanged()
{
	const auto& values = mReaders.values();
	for (BluetoothReader* const reader : values)
	{
		const bool connected = reader->getReaderInfo().isConnected();
		if (mConnectToKnownReaders && !connected)
		{
			/*
			 * Workaround for pairing problem on Android:
			 *
			 * The reader detection process performs a BT connect, determines the services and characteristics
			 * and disconnects afterwards. When selecting a reader we connect again.
			 * This causes some timing issue. Other developers have similar problems, e.g. see
			 * https://github.com/NordicSemiconductor/Android-DFU-Library/issues/1#issuecomment-156790789
			 *
			 * This led to setting a delay of 1000 msecs.
			 */
			QTimer::singleShot(1000, [ = ] {
						reader->connectReader();
					});
		}
		else if (connected && !mConnectToKnownReaders)
		{
			reader->disconnectReader();
		}
	}
}


void BluetoothReaderManagerPlugIn::onDeviceDiscovered(const QBluetoothDeviceInfo& pInfo)
{
	setReaderInfoResponding(true);
	QString deviceId = BluetoothDeviceUtil::getDeviceId(pInfo);
	if (mReaders.contains(deviceId))
	{
		if (!mReadersDiscoveredInCurrentScan.contains(deviceId))
		{
			qCDebug(bluetooth) << "Bluetooth device re-discovered" << pInfo;
			mReadersDiscoveredInCurrentScan += deviceId;
		}
		return;
	}

	if (mInitializingDevices.contains(deviceId))
	{
		auto knownDevice = mInitializingDevices.value(deviceId);
		knownDevice->setDeviceInfo(pInfo);
		return;
	}

	qCDebug(bluetooth) << "Bluetooth device discovered" << pInfo;

	QSharedPointer<CyberJackWaveDevice> newDevice(new CyberJackWaveDevice(pInfo));
	mInitializingDevices.insert(deviceId, newDevice);
	connect(newDevice.data(), &CyberJackWaveDevice::fireInitialized, this, &BluetoothReaderManagerPlugIn::onDeviceInitialized);
	newDevice->initialize();
}


void BluetoothReaderManagerPlugIn::onDeviceInitialized(const QBluetoothDeviceInfo& pInfo)
{
	const QString& deviceId = BluetoothDeviceUtil::getDeviceId(pInfo);
	auto device = mInitializingDevices.value(deviceId);
	if (!device)
	{
		return;
	}
	disconnect(device.data(), &CyberJackWaveDevice::fireInitialized, this, &BluetoothReaderManagerPlugIn::onDeviceInitialized);

	const ScopeGuard disconnector([device] {
				device->disconnectFromDevice();
			});

	if (!device->isValid())
	{
		return;
	}

	Q_ASSERT_X(!mReaders.contains(deviceId), "BluetoothReaderManagerPlugIn", "Device is already determined as reader");

	BluetoothReader* reader = new BluetoothReader(device);
	qCDebug(bluetooth) << "Device is successfully initialized, create reader" << reader->getName();

	connect(reader, &BluetoothReader::fireReaderConnected, this, &ReaderManagerPlugIn::fireReaderAdded);
	connect(reader, &Reader::fireCardInserted, this, &ReaderManagerPlugIn::fireCardInserted);
	connect(reader, &Reader::fireCardRemoved, this, &ReaderManagerPlugIn::fireCardRemoved);
	connect(reader, &Reader::fireCardRetryCounterChanged, this, &ReaderManagerPlugIn::fireCardRetryCounterChanged);
	connect(reader, &Reader::fireReaderDeviceError, this, &ReaderManagerPlugIn::fireReaderDeviceError);

	mReadersDiscoveredInCurrentScan += deviceId;
	mReaders.insert(deviceId, reader);
	connect(device.data(), &CyberJackWaveDevice::fireDisconnected, this, &BluetoothReaderManagerPlugIn::onDeviceDisconnected);

	if (mConnectToKnownReaders)
	{
		/*
		 * Workaround for pairing problem on Android:
		 *
		 * The reader detection process performs a BT connect, determines the services and characteristics
		 * and disconnects afterwards. When selecting a reader we connect again.
		 * This causes some timing issue. Other developers have similar problems, e.g. see
		 * https://github.com/NordicSemiconductor/Android-DFU-Library/issues/1#issuecomment-156790789
		 *
		 * This led to setting a delay of 1000 msecs.
		 */
		QTimer::singleShot(1000, [ = ] {
					reader->connectReader();
				});
	}
}


void BluetoothReaderManagerPlugIn::onDeviceDisconnected(const QBluetoothDeviceInfo& pInfo)
{
	const QString& deviceId = BluetoothDeviceUtil::getDeviceId(pInfo);
	auto device = mInitializingDevices.value(deviceId);
	if (!device)
	{
		return;
	}
	disconnect(device.data(), &CyberJackWaveDevice::fireDisconnected, this, &BluetoothReaderManagerPlugIn::onDeviceDisconnected);

	if (auto reader = mReaders.value(deviceId))
	{
		qCDebug(bluetooth) << "Device is disconnected" << reader->getName();
		Q_EMIT fireReaderRemoved(reader->getName());
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

	if (pError == QBluetoothDeviceDiscoveryAgent::InputOutputError)
	{
		setReaderInfoResponding(false);
	}

	Q_EMIT fireReaderDeviceError(pError == QBluetoothDeviceDiscoveryAgent::PoweredOffError ? DeviceError::DEVICE_POWERED_OFF : DeviceError::DEVICE_SCAN_ERROR);
}


void BluetoothReaderManagerPlugIn::onDeviceDiscoveryCanceled()
{
	qCDebug(bluetooth) << "Bluetooth device discovery canceled";

	Q_D(BluetoothReaderManagerPlugIn);
	d->onDeviceDiscoveryCanceled();

	mInitializingDevices.clear();
	mReadersDiscoveredInCurrentScan.clear();
}


void BluetoothReaderManagerPlugIn::onRemoveReader(const QString& pDeviceId)
{
	if (auto reader = mReaders.take(pDeviceId))
	{
		Q_EMIT fireReaderRemoved(reader->getName());
		delete reader;
	}
}


void BluetoothReaderManagerPlugIn::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mTimerIdDiscoverPairedDevices)
	{
		Q_D(BluetoothReaderManagerPlugIn);
		d->handlePairedDevices();
		return;
	}

	ReaderManagerPlugIn::timerEvent(pEvent);
}
