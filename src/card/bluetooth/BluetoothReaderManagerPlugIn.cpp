/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothReaderManagerPlugIn.h"

#include "AndroidBluetoothAdapter.h"
#include "BluetoothDebug.h"
#include "BluetoothDeviceUtil.h"
#include "BluetoothReader.h"
#include "BluetoothReaderManagerPlugIn_p.h"
#include "GlobalStatus.h"
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


void BluetoothReaderManagerPlugIn::startScan(bool /*pAutoConnect*/)
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
		mDeviceDiscoveryAgent.start();
	}

	setScanInProgress(true);
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

	setScanInProgress(false);
}


void BluetoothReaderManagerPlugIn::setScanInProgress(bool pScanInProgress)
{
	if (mScanInProgress != pScanInProgress)
	{
		mScanInProgress = pScanInProgress;
		onScanInProgressChanged();
	}
}


void BluetoothReaderManagerPlugIn::onScanInProgressChanged()
{
	const auto& values = mReaders.values();
	for (BluetoothReader* const reader : values)
	{
		const bool connected = reader->getReaderInfo().isConnected();
		if (mScanInProgress && !connected)
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
			QTimer::singleShot(1000, reader, [ = ] {
						reader->connectReader();
					});
		}
		else if (connected && !mScanInProgress)
		{
			reader->disconnectReader();
		}
	}
}


QVector<QString> BluetoothReaderManagerPlugIn::deviceIdsForReaderName(const QString& pReaderName)
{
	QVector<QString> result;

	for (QMap<QString, BluetoothReader*>::const_iterator it = mReaders.constBegin(); it != mReaders.constEnd(); ++it)
	{
		const QString& deviceId = it.key();
		const Reader* reader = it.value();
		if (reader->getName() == pReaderName)
		{
			result += deviceId;
		}
	}

	return result;
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

	const auto& newDevice = QSharedPointer<CyberJackWaveDevice>::create(pInfo);
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

	connect(reader, &BluetoothReader::fireReaderConnected, this, &BluetoothReaderManagerPlugIn::onReaderConnected);
	connect(reader, &BluetoothReader::fireReaderConnectionFailed, this, &BluetoothReaderManagerPlugIn::onReaderConnectionFailed);
	connect(reader, &Reader::fireCardInserted, this, &ReaderManagerPlugIn::fireCardInserted);
	connect(reader, &Reader::fireCardRemoved, this, &BluetoothReaderManagerPlugIn::onCardRemoved);
	connect(reader, &Reader::fireCardRetryCounterChanged, this, &ReaderManagerPlugIn::fireCardRetryCounterChanged);
	connect(reader, &Reader::fireReaderDeviceError, this, &ReaderManagerPlugIn::fireReaderDeviceError);

	mReadersDiscoveredInCurrentScan += deviceId;
	mReaders.insert(deviceId, reader);
	connect(device.data(), &CyberJackWaveDevice::fireDisconnected, this, &BluetoothReaderManagerPlugIn::onDeviceDisconnected);

	if (mScanInProgress)
	{
		mPendingConnections.insert(reader->getName(), 1);

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
		QTimer::singleShot(1000, reader, [ = ] {
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

	Q_EMIT fireReaderDeviceError(pError == QBluetoothDeviceDiscoveryAgent::PoweredOffError ? GlobalStatus::Code::No_Error : GlobalStatus::Code::Workflow_Reader_Device_Scan_Error);
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


void BluetoothReaderManagerPlugIn::onReaderConnected(const QString& pReaderName)
{
	Q_EMIT fireReaderAdded(pReaderName);
}


void BluetoothReaderManagerPlugIn::onCardRemoved(const QString& pReaderName)
{
	const QVector<QString> ids = deviceIdsForReaderName(pReaderName);
	if (ids.isEmpty())
	{
		qCWarning(bluetooth) << "Card removed for unknown reader:" << pReaderName;
		return;
	}

	for (const QString& id : ids)
	{
		const BluetoothReader* const reader = mReaders.value(id);
		const bool connected = reader->getReaderInfo().isConnected();

		Q_EMIT fireCardRemoved(pReaderName);
		if (!connected)
		{
			qCDebug(bluetooth) << "Device is disconnected:" << pReaderName;
			Q_EMIT fireReaderRemoved(pReaderName);
		}
	}
}


void BluetoothReaderManagerPlugIn::onReaderConnectionFailed(const QString& pReaderName)
{
	if (!mPendingConnections.contains(pReaderName))
	{
		qCWarning(bluetooth) << "Connection failed for a reader with no pending connection:" << pReaderName;

		return;
	}

	const int attemptCount = mPendingConnections.value(pReaderName);
	if (attemptCount >= 2)
	{
		qCWarning(bluetooth) << "Maximum connection attempt count reached for reader:" << pReaderName;

		return;
	}

	BluetoothReader* const reader = mReaders.value(pReaderName, nullptr);
	if (reader)
	{
		mPendingConnections.insert(pReaderName, attemptCount + 1);

		/*
		 * Sometimes a connection failure occurs after a successful
		 * pairing. In this case, the bluetooth device is connected but
		 * does not report a successful connection.
		 * Before we try to reconnect to the device, it is necessary
		 * to disconnect from it.
		 */
		reader->disconnectReader();

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
		QTimer::singleShot(1000, reader, [ = ] {
					reader->connectReader();
				});
	}
	else
	{
		qCWarning(bluetooth) << "Cannot reconnect to reader, reader not found:" << pReaderName;
	}
}
