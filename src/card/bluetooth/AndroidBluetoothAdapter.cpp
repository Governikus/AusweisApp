/*!
 * AndroidBluetoothAdapter.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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

QBluetoothDeviceInfo::CoreConfiguration AndroidBluetoothAdapter::fromAndroidDeviceType(quint32 pAndroidDeviceTypeConstant)
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


AndroidBluetoothAdapter::AndroidBluetoothAdapter(bool pStateOn, const QVector<QBluetoothDeviceInfo>& pKnownBluetoothDevices)
	: mStateOn(pStateOn)
	, mBondedDevices(pKnownBluetoothDevices)
{
}


AndroidBluetoothAdapter AndroidBluetoothAdapter::getDefaultAdapter()
{
	bool stateOn = false;
	QVector<QBluetoothDeviceInfo> bondedDevices;

#ifdef Q_OS_ANDROID
	QAndroidJniEnvironment env;
	jclass adapterClass = env->FindClass("android/bluetooth/BluetoothAdapter");
	if (adapterClass == nullptr)
	{
		return AndroidBluetoothAdapter();
	}

	jmethodID defaultAdapter = env->GetStaticMethodID(adapterClass, "getDefaultAdapter", "()Landroid/bluetooth/BluetoothAdapter;");
	if (defaultAdapter == nullptr)
	{
		env->DeleteLocalRef(adapterClass);
		return AndroidBluetoothAdapter();
	}

	jobject adapterObject = env->CallStaticObjectMethod(adapterClass, defaultAdapter);
	if (adapterObject == nullptr)
	{
		env->DeleteLocalRef(adapterClass);
		return AndroidBluetoothAdapter();
	}

	QAndroidJniObject o(adapterObject);
	if (!o.isValid())
	{
		env->DeleteLocalRef(adapterObject);
		env->DeleteLocalRef(adapterClass);
		return AndroidBluetoothAdapter();
	}

	stateOn = (o.callMethod<jint>("getState") == STATE_ON);

	QAndroidJniObject deviceSet = o.callObjectMethod("getBondedDevices", "()Ljava/util/Set;");
	for (QAndroidJniObject iter = deviceSet.callObjectMethod("iterator", "()Ljava/util/Iterator;"); (bool) iter.callMethod<jboolean>("hasNext", "()Z"); )
	{
		QAndroidJniObject device = iter.callObjectMethod("next", "()Ljava/lang/Object;");

		QString name = device.callObjectMethod<jstring>("getName").toString();
		QString address = device.callObjectMethod<jstring>("getAddress").toString();
		QAndroidJniObject bluetoothClass = device.callObjectMethod("getBluetoothClass", "()Landroid/bluetooth/BluetoothClass;");
		quint32 deviceClass = bluetoothClass.callMethod<jint>("getDeviceClass");
		quint32 type = device.callMethod<jint>("getType");

		QBluetoothDeviceInfo deviceInfo(QBluetoothAddress(address), name, deviceClass);
		deviceInfo.setCoreConfigurations(fromAndroidDeviceType(type));
		deviceInfo.setCached(true);
		bondedDevices.append(deviceInfo);
	}

	env->DeleteLocalRef(adapterObject);
	env->DeleteLocalRef(adapterClass);
#else
	Q_UNUSED(STATE_ON)
#endif

	return AndroidBluetoothAdapter(stateOn, bondedDevices);
}


QVector<QBluetoothDeviceInfo> AndroidBluetoothAdapter::getBondedDevices() const
{
	return mBondedDevices;
}


bool AndroidBluetoothAdapter::isStateOn() const
{
	return mStateOn;
}
