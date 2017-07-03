/*!
 * BluetoothReaderManagerPlugInPrivate_p_android.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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
		QMetaObject::invokeMethod(instance, "onBluetoothStatusChanged", Qt::BlockingQueuedConnection, Q_ARG(bool, false));
	}
	else if (currentState == 12) // STATE_ON (class android.bluetooth.BluetoothAdapter)
	{
		qCDebug(bluetooth) << "Bluetooth was powered on";
		// jump from Android thread into Qt thread by invoking per BlockingQueuedConnection
		QMetaObject::invokeMethod(instance, "onBluetoothStatusChanged", Qt::BlockingQueuedConnection, Q_ARG(bool, true));
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
	q->setBluetoothStatus(AndroidBluetoothAdapter::getDefaultAdapter().isStateOn());
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
	auto pairedDevices = AndroidBluetoothAdapter::getDefaultAdapter().getBondedDevices();
	if (pairedDevices.isEmpty())
	{
		qCDebug(bluetooth) << "No paired devices found";
	}
	else
	{
		qCDebug(bluetooth) << "Found" << pairedDevices.size() << "paired devices";
		Q_Q(BluetoothReaderManagerPlugIn);
		for (QBluetoothDeviceInfo pairedDevice : pairedDevices)
		{
			q->onDeviceDiscovered(pairedDevice);
		}
	}
}


void BluetoothReaderManagerPlugInPrivate::onDeviceDiscoveryCanceled()
{
	Q_Q(BluetoothReaderManagerPlugIn);
	for (auto deviceId : q->mReaders.keys())
	{
		if (!q->mDiscoveredReadersInCurrentScan.contains(deviceId))
		{
			q->onRemoveReader(deviceId);
		}
	}
}
