/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "ReaderDetector.h"

#ifdef HAVE_LIBUDEV
#include <libudev.h>
#endif

#include <QByteArray>
#include <QLoggingCategory>
#include <QPair>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_drivers)


class DeviceListener
	: public QThread
{
	Q_OBJECT

	private:
#ifdef HAVE_LIBUDEV
		struct udev* mUserDevices;
		struct udev_monitor* mDeviceMonitor;
		int mFileDescriptor;
#endif

		void run() override
		{
#ifdef HAVE_LIBUDEV
			for (;; )
			{
				fd_set fds;
				FD_ZERO(&fds);
				FD_SET(mFileDescriptor, &fds);

				int ret = select(mFileDescriptor + 1, &fds, nullptr, nullptr, nullptr);

				// Check if our file descriptor has received data
				if (ret > 0 && FD_ISSET(mFileDescriptor, &fds))
				{
					// Clear the recognized changes
					udev_monitor_receive_device(mDeviceMonitor);

					qCDebug(card_drivers) << "System information: device changed";

					Q_EMIT fireDeviceChangeDetected();
				}
			}
#endif
		}


	public:
		DeviceListener()
		{
#ifdef HAVE_LIBUDEV
			mUserDevices = udev_new();

			// Set up a monitor to monitor usb devices
			mDeviceMonitor = udev_monitor_new_from_netlink(mUserDevices, "udev");
			udev_monitor_filter_add_match_subsystem_devtype(mDeviceMonitor, "usb", nullptr);
			udev_monitor_enable_receiving(mDeviceMonitor);

			/// Get the file descriptor (fd) for the monitor
			mFileDescriptor = udev_monitor_get_fd(mDeviceMonitor);
#endif
		}


		~DeviceListener()
		{
#ifdef HAVE_LIBUDEV
			udev_monitor_unref(mDeviceMonitor);
			udev_unref(mUserDevices);
#endif
		}


	Q_SIGNALS:
		void fireDeviceChangeDetected();
};


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
	mDeviceListener->terminate();
	mDeviceListener->wait();
	delete mDeviceListener;

	return true;
}


QVector<QPair<uint, uint> > ReaderDetector::attachedDevIds() const
{
	QVector<QPair<uint, uint> > result;

#ifdef HAVE_LIBUDEV
	// http://www.signal11.us/oss/udev/

	struct udev* udev;
	struct udev_enumerate* enumerate;
	struct udev_list_entry* devices, * dev_list_entry;

	/* Create the udev object */
	udev = udev_new();
	if (!udev)
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
		const char* path = udev_list_entry_get_name(dev_list_entry);
		struct udev_device* dev = udev_device_new_from_syspath(udev, path);

		/* usb_device_get_devnode() returns the path to the device node
		   itself in /dev. */
		qCDebug(card_drivers) << "Device Node Path:" << udev_device_get_devnode(dev);

		/* The device pointed to by dev contains information about
		   the hidraw device. In order to get information about the
		   USB device, get the parent device with the
		   subsystem/devtype pair of "usb"/"usb_device". This will
		   be several levels up the tree, but the function will find
		   it.*/
		dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
		if (!dev)
		{
			qCDebug(card_drivers) << "Unable to find parent usb device.";
			continue;
		}

		/* From here, we can call get_sysattr_value() for each file
		   in the device's /sys entry. The strings passed into these
		   functions (idProduct, idVendor, serial, etc.) correspond
		   directly to the files in the directory which represents
		   the USB device. Note that USB strings are Unicode, UCS2
		   encoded, but the strings returned from
		   udev_device_get_sysattr_value() are UTF-8 encoded. */
		QByteArray vid(udev_device_get_sysattr_value(dev, "idVendor"));
		QByteArray pid(udev_device_get_sysattr_value(dev, "idProduct"));
		result += QPair<uint, uint>(vid.toUInt(nullptr, 16), pid.toUInt(nullptr, 16));
		qCDebug(card_drivers) << "VID / PID:" << vid << "/" << pid;
		qCDebug(card_drivers) << "Manufacturer:" << udev_device_get_sysattr_value(dev, "manufacturer");
		qCDebug(card_drivers) << "Product:" << udev_device_get_sysattr_value(dev, "product");

		udev_device_unref(dev);
	}
	/* Free the enumerator object */
	udev_enumerate_unref(enumerate);

	udev_unref(udev);
#endif

	return result;
}


#include "ReaderDetector_linux.moc"
