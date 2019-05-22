/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AndroidBluetoothAdapter.h"

#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

using namespace governikus;

/*
 * Constants from class android.bluetooth.BluetoothDevice
 */
const int DEVICE_TYPE_CLASSIC = 1;
const int DEVICE_TYPE_DUAL = 3;
const int DEVICE_TYPE_LE = 2;

/*
 * Constant from class android.bluetooth.BluetoothAdapter
 */
const int STATE_ON = 12;

QBluetoothDeviceInfo::CoreConfiguration AndroidBluetoothAdapter::fromAndroidDeviceType(int pAndroidDeviceTypeConstant)
{
	switch (pAndroidDeviceTypeConstant)
	{
		case DEVICE_TYPE_CLASSIC:
			return QBluetoothDeviceInfo::CoreConfiguration::BaseRateCoreConfiguration;

		case DEVICE_TYPE_DUAL:
			return QBluetoothDeviceInfo::CoreConfiguration::BaseRateAndLowEnergyCoreConfiguration;

		case DEVICE_TYPE_LE:
			return QBluetoothDeviceInfo::CoreConfiguration::LowEnergyCoreConfiguration;

		default:
			return QBluetoothDeviceInfo::CoreConfiguration::UnknownCoreConfiguration;
	}
}


AndroidBluetoothAdapter::AndroidBluetoothAdapter(bool pAvailable,
		bool pStateOn,
		const QVector<QBluetoothDeviceInfo>& pKnownBluetoothDevices)
	: mAvailable(pAvailable)
	, mStateOn(pStateOn)
	, mBondedDevices(pKnownBluetoothDevices)
{
}


AndroidBluetoothAdapter AndroidBluetoothAdapter::getDefaultAdapter()
{
	bool available = false;
	bool stateOn = false;
	QVector<QBluetoothDeviceInfo> bondedDevices;

#ifdef Q_OS_ANDROID
	QAndroidJniObject adapter = QAndroidJniObject::callStaticObjectMethod("android/bluetooth/BluetoothAdapter", "getDefaultAdapter", "()Landroid/bluetooth/BluetoothAdapter;");

	if (!adapter.isValid())
	{
		return AndroidBluetoothAdapter();
	}

	available = true;
	stateOn = (adapter.callMethod<jint>("getState") == STATE_ON);

	QAndroidJniObject deviceSet = adapter.callObjectMethod("getBondedDevices", "()Ljava/util/Set;");
	for (QAndroidJniObject iter = deviceSet.callObjectMethod("iterator", "()Ljava/util/Iterator;")
			; static_cast<bool>(iter.callMethod<jboolean>("hasNext", "()Z"))
			;)
	{
		QAndroidJniObject device = iter.callObjectMethod("next", "()Ljava/lang/Object;");

		QString name = device.callObjectMethod<jstring>("getName").toString();
		QString address = device.callObjectMethod<jstring>("getAddress").toString();
		QAndroidJniObject bluetoothClass = device.callObjectMethod("getBluetoothClass", "()Landroid/bluetooth/BluetoothClass;");
		int deviceClass = bluetoothClass.callMethod<jint>("getDeviceClass");
		int type = device.callMethod<jint>("getType");

		QBluetoothDeviceInfo deviceInfo(QBluetoothAddress(address), name, static_cast<quint32>(deviceClass));
		deviceInfo.setCoreConfigurations(fromAndroidDeviceType(type));
		deviceInfo.setCached(true);
		bondedDevices += deviceInfo;
	}
#else
	Q_UNUSED(STATE_ON)
#endif

	return AndroidBluetoothAdapter(available, stateOn, bondedDevices);
}


QVector<QBluetoothDeviceInfo> AndroidBluetoothAdapter::getBondedDevices() const
{
	return mBondedDevices;
}


bool AndroidBluetoothAdapter::isStateOn() const
{
	return mStateOn;
}


bool AndroidBluetoothAdapter::isAvailable() const
{
	return mAvailable;
}
