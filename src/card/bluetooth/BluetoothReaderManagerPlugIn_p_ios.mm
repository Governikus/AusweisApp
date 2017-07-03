/*!
 * BluetoothReaderManagerPlugInPrivate_p_ios.mm
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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
	qRegisterMetaType<QBluetoothDeviceInfo>("QBluetoothDeviceInfo");
	qRegisterMetaType<QLowEnergyCharacteristic>("QLowEnergyCharacteristic");
	qRegisterMetaType<QLowEnergyDescriptor>("QLowEnergyDescriptor");
}


void BluetoothReaderManagerPlugInPrivate::init()
{
	Q_Q(BluetoothReaderManagerPlugIn);
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
