/*!
 * \brief Private implementation part of the ReaderManagerPlugIn with Qt'S Bluetooth API.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
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

	BluetoothReaderManagerPlugIn* const q_ptr;
	explicit BluetoothReaderManagerPlugInPrivate(BluetoothReaderManagerPlugIn* pPublic);

	public Q_SLOTS:
		void onBluetoothStatusChanged(bool pEnabled);

	private Q_SLOTS:
		void init();
		void onScanStart();
		void handlePairedDevices();
		void onDeviceDiscoveryCanceled();
};


} // namespace governikus
