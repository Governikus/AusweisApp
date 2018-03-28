/*!
 * \brief Utility function for determination of unique Bluetooth device ids.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>


class BluetoothDeviceUtil
{
	private:
		BluetoothDeviceUtil() = delete;
		~BluetoothDeviceUtil() = delete;

	public:
		/*!
		 * According to the Qt documentation:
		 *
		 * "CoreBluetooth API hides addresses and provides unique UUIDs to identify a device.
		 * This UUID is not the same thing as a service UUID and is required to work later with
		 * CoreBluetooth API and discovered devices."
		 *
		 * So we use the device UUID or the Bluetooth address as device id depending on the
		 * operating system we are on.
		 */
		static QString getDeviceId(const QBluetoothDeviceInfo& pInfo)
		{
			#if defined(Q_OS_IOS) || defined(Q_OS_X)
			return pInfo.deviceUuid().toString();

			#else
			return pInfo.address().toString();

			#endif
		}


};
