/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothReaderManagerPlugIn_p.h"

using namespace governikus;

BluetoothReaderManagerPlugInPrivate::BluetoothReaderManagerPlugInPrivate(BluetoothReaderManagerPlugIn* pPublic)
	: QObject(pPublic)
	, q_ptr(pPublic)
{
}


void BluetoothReaderManagerPlugInPrivate::init()
{
}


void BluetoothReaderManagerPlugInPrivate::onBluetoothStatusChanged(bool)
{
}


void BluetoothReaderManagerPlugInPrivate::onScanStart()
{
}


void BluetoothReaderManagerPlugInPrivate::handlePairedDevices()
{
}


void BluetoothReaderManagerPlugInPrivate::onDeviceDiscoveryCanceled()
{
}
