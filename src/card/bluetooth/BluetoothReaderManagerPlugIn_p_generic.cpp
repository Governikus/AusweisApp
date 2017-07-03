/*!
 * BluetoothReaderManagerPlugInPrivate_p_generic.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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
