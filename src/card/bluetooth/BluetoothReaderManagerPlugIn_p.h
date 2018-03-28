/*!
 * \brief Private implementation part of the ReaderManagerPlugIn with Qt'S Bluetooth API.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BluetoothReaderManagerPlugIn.h"

namespace governikus
{

class BluetoothReaderManagerPlugInPrivate
	: public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(BluetoothReaderManagerPlugIn)

	BluetoothReaderManagerPlugIn * const q_ptr;
	BluetoothReaderManagerPlugInPrivate(BluetoothReaderManagerPlugIn* pPublic);

	private Q_SLOTS:
		void init();
		void onBluetoothStatusChanged(bool pEnabled);
		void onScanStart();
		void handlePairedDevices();
		void onDeviceDiscoveryCanceled();
};


} /* namespace governikus */
