/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
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
	q->setReaderInfoAvailable(true);
	q->setBluetoothStatus(true);
}


void BluetoothReaderManagerPlugInPrivate::onBluetoothStatusChanged(bool)
{
}


void BluetoothReaderManagerPlugInPrivate::onScanStart()
{
	NSMutableDictionary* options = [[[NSMutableDictionary alloc] init] autorelease];
	[options setObject:[NSNumber numberWithBool:YES] forKey:CBCentralManagerOptionShowPowerAlertKey];
	[[[CBCentralManager alloc] initWithDelegate:nil queue:nil options:options] autorelease];
}


void BluetoothReaderManagerPlugInPrivate::handlePairedDevices()
{
}


void BluetoothReaderManagerPlugInPrivate::onDeviceDiscoveryCanceled()
{
}
