/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "AndroidBluetoothAdapter.h"
#include "BluetoothReaderManagerPlugIn_p.h"

#include <jni.h>

#include <QLoggingCategory>
#include <QPointer>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>

Q_DECLARE_LOGGING_CATEGORY(bluetooth)

using namespace governikus;

static QPointer<BluetoothReaderManagerPlugInPrivate> instance;

extern "C"
{
JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_AndroidBluetoothReceiver_bluetoothAdapterStateChanged(JNIEnv* env, jobject obj, jint previousState, jint currentState)
{
	Q_UNUSED(env)
	Q_UNUSED(obj)
	Q_UNUSED(previousState)

	if (instance == nullptr)
	{
		qCWarning(bluetooth) << "BluetoothReaderManagerPlugIn not yet instantiated, skip processing";
		return;
	}

	if (currentState == 10) // STATE_OFF (class android.bluetooth.BluetoothAdapter)
	{
		qCDebug(bluetooth) << "Bluetooth was powered off";
		// jump from Android thread into Qt thread by invoking per BlockingQueuedConnection
		QMetaObject::invokeMethod(instance, [] {
					instance->onBluetoothStatusChanged(false);
				}, Qt::BlockingQueuedConnection);
	}
	else if (currentState == 12) // STATE_ON (class android.bluetooth.BluetoothAdapter)
	{
		qCDebug(bluetooth) << "Bluetooth was powered on";
		// jump from Android thread into Qt thread by invoking per BlockingQueuedConnection
		QMetaObject::invokeMethod(instance, [] {
					instance->onBluetoothStatusChanged(true);
				}, Qt::BlockingQueuedConnection);
	}
}


}


BluetoothReaderManagerPlugInPrivate::BluetoothReaderManagerPlugInPrivate(BluetoothReaderManagerPlugIn* pPublic)
	: QObject(pPublic)
	, q_ptr(pPublic)
{
	Q_ASSERT(!instance);
	instance = this;
}


void BluetoothReaderManagerPlugInPrivate::init()
{
	Q_Q(BluetoothReaderManagerPlugIn);
	const auto& adapter = AndroidBluetoothAdapter::getDefaultAdapter();
	q->setReaderInfoAvailable(adapter.isAvailable());
	q->setBluetoothStatus(adapter.isStateOn());
}


void BluetoothReaderManagerPlugInPrivate::onBluetoothStatusChanged(bool pEnabled)
{
	Q_Q(BluetoothReaderManagerPlugIn);
	q->setBluetoothStatus(pEnabled);
	if (pEnabled)
	{
		handlePairedDevices();
	}
}


void BluetoothReaderManagerPlugInPrivate::onScanStart()
{
}


void BluetoothReaderManagerPlugInPrivate::handlePairedDevices()
{
	const QVector<QBluetoothDeviceInfo>& pairedDevices = AndroidBluetoothAdapter::getDefaultAdapter().getBondedDevices();
	for (const QBluetoothDeviceInfo& device : pairedDevices)
	{
		Q_Q(BluetoothReaderManagerPlugIn);
		q->onDeviceDiscovered(device);
	}
}


void BluetoothReaderManagerPlugInPrivate::onDeviceDiscoveryCanceled()
{
	Q_Q(BluetoothReaderManagerPlugIn);
	for (auto deviceId : q->mReaders.keys())
	{
		if (!q->mReadersDiscoveredInCurrentScan.contains(deviceId))
		{
			q->onRemoveReader(deviceId);
		}
	}
}
