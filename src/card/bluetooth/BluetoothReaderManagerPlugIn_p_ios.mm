/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothReaderManagerPlugIn_p.h"

#include <QMetaType>

#import <CoreBluetooth/CoreBluetooth.h>
#import <Foundation/Foundation.h>

using namespace governikus;


BluetoothReaderManagerPlugInPrivate::BluetoothReaderManagerPlugInPrivate(BluetoothReaderManagerPlugIn* pPublic)
	: QObject(pPublic)
	, q_ptr(pPublic)
{
}


void BluetoothReaderManagerPlugInPrivate::init()
{
	Q_Q(BluetoothReaderManagerPlugIn);
	q->setPlugInAvailable(true);
	q->setBluetoothStatus(true);
}


void BluetoothReaderManagerPlugInPrivate::onBluetoothStatusChanged(bool)
{
}


void BluetoothReaderManagerPlugInPrivate::onScanStart()
{
	NSMutableDictionary* options = [[[NSMutableDictionary alloc] init] autorelease];
	options[CBCentralManagerOptionShowPowerAlertKey] = @YES; // Display a dialog to the user if Bluetooth is off
	[[[CBCentralManager alloc] initWithDelegate:nil queue:nil options:options] autorelease];
}


void BluetoothReaderManagerPlugInPrivate::handlePairedDevices()
{
}


void BluetoothReaderManagerPlugInPrivate::onDeviceDiscoveryCanceled()
{
}
