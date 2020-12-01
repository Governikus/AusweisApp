/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDetector.h"

#include <QByteArray>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_drivers)

bool ReaderDetector::initNativeEvents()
{
	mDeviceListener = new DeviceListener();
	connect(mDeviceListener, &DeviceListener::fireDeviceChangeDetected, this, &ReaderDetector::fireReaderChangeDetected);
	mDeviceListener->start();

	return true;
}


bool ReaderDetector::terminateNativeEvents()
{
	disconnect(mDeviceListener, &DeviceListener::fireDeviceChangeDetected, this, &ReaderDetector::fireReaderChangeDetected);
	mDeviceListener->requestInterruption();
	const int waitForMilliseconds = 3 * 1000;
	mDeviceListener->wait(waitForMilliseconds);
	if (mDeviceListener->isRunning())
	{
		qCDebug(card_drivers) << "Terminating device listener thread.";
		mDeviceListener->terminate();
	}
	delete mDeviceListener;

	return true;
}


QVector<UsbId> ReaderDetector::attachedDevIds() const
{
	QVector<UsbId> result;

#if __has_include(<libudev.h>)
	// http://www.signal11.us/oss/udev/

	struct udev* udev;
	struct udev_enumerate* enumerate;
	struct udev_list_entry* devices, * dev_list_entry;

	/* Create the udev object */
	udev = udev_new();
	if (udev == nullptr)
	{
		qCDebug(card_drivers) << "Can't create udev";
		return result;
	}

	/* Create a list of the devices in the 'usb' subsystem. */
	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "usb");
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);

	/* For each item enumerated, print out its information.
	   udev_list_entry_foreach is a macro which expands to
	   a loop. The loop will be executed for each member in
	   devices, setting dev_list_entry to a list entry
	   which contains the device's path in /sys. */
	udev_list_entry_foreach(dev_list_entry, devices)
	{
		/* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
		const char* const path = udev_list_entry_get_name(dev_list_entry);
		struct udev_device* dev = udev_device_new_from_syspath(udev, path);

		/* The device pointed to by dev contains information about
		   the hidraw device. In order to get information about the
		   USB device, get the parent device with the
		   subsystem/devtype pair of "usb"/"usb_device". This will
		   be several levels up the tree, but the function will find
		   it.*/
		auto* parent_dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
		if (parent_dev == nullptr)
		{
			udev_device_unref(dev);
			continue;
		}

		/* From here, we can call get_sysattr_value() for each file
		   in the device's /sys entry. The strings passed into these
		   functions (idProduct, idVendor, serial, etc.) correspond
		   directly to the files in the directory which represents
		   the USB device. Note that USB strings are Unicode, UCS2
		   encoded, but the strings returned from
		   udev_device_get_sysattr_value() are UTF-8 encoded. */
		QByteArray vid(udev_device_get_sysattr_value(parent_dev, "idVendor"));
		QByteArray pid(udev_device_get_sysattr_value(parent_dev, "idProduct"));
		const UsbId usbId(vid.toUInt(nullptr, 16), pid.toUInt(nullptr, 16));
		result += usbId;

		udev_device_unref(dev);
	}
	/* Free the enumerator object */
	udev_enumerate_unref(enumerate);

	udev_unref(udev);
#endif

	return result;
}
