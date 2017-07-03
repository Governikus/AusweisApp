/*!
 * AndroidBluetoothAdapter.h
 *
 * \brief This class accesses the java class android.bluetooth.BluetoothAdapter
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once


#include <QBluetoothDeviceInfo>
#include <QVector>


namespace governikus
{

class AndroidBluetoothAdapter
{
	private:
		static QBluetoothDeviceInfo::CoreConfiguration fromAndroidDeviceType(quint32 pAndroidDeviceTypeConstant);

		bool mAvailable;
		bool mStateOn;
		QVector<QBluetoothDeviceInfo> mBondedDevices;

		AndroidBluetoothAdapter(bool pAvailable = false,
				bool pStateOn = false,
				const QVector<QBluetoothDeviceInfo>& pKnownBluetoothDevices = QVector<QBluetoothDeviceInfo>());

	public:
		/*!
		 * Get the default bluetooth adapter.
		 */
		static AndroidBluetoothAdapter getDefaultAdapter();

		/*!
		 * Return the set of BluetoothDeviceInfo objects that are bonded (paired) to the local adapter.
		 */
		QVector<QBluetoothDeviceInfo> getBondedDevices() const;

		/*!
		 * Returns true, if bluetooth is turned on.
		 */
		bool isStateOn() const;

		bool isAvailable() const;
};

} /* namespace governikus */
