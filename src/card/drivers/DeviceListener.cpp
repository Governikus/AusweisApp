/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "DeviceListener.h"

#if __has_include(<libudev.h>)
	#include <libudev.h>
#endif

#include <QByteArray>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QPair>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_drivers)

void DeviceListener::run()
{
#if __has_include(<libudev.h>)
	for (;;)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(mFileDescriptor, &fds);

		// On Linux, select() modifies timeout to reflect the amount of time not
		// slept; most other implementations do not do this.  (POSIX.1 permits
		// either behavior.)
		struct timeval timeout;
	#ifndef QT_NO_DEBUG
		if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
		{
			timeout = {
				0, /*long tv_sec*/
				100 /*long tv_usec*/
			};
		}
		else
	#endif
		{
			timeout = {
				1, /*long tv_sec*/
				0 /*long tv_usec*/
			};
		}

		int ret = select(mFileDescriptor + 1, &fds, nullptr, nullptr, &timeout);

		// Check if our file descriptor has received data
		if (ret > 0 && FD_ISSET(mFileDescriptor, &fds))
		{
			// Clear the recognized changes
			udev_monitor_receive_device(mDeviceMonitor);

			qCDebug(card_drivers) << "System information: device changed";

			Q_EMIT fireDeviceChangeDetected();
		}

		if (isInterruptionRequested())
		{
			qCDebug(card_drivers) << "Thread interruption requested.";
			break;
		}
	}
#endif
}


DeviceListener::DeviceListener()
{
#if __has_include(<libudev.h>)
	mUserDevices = udev_new();

	// Set up a monitor to monitor usb devices
	mDeviceMonitor = udev_monitor_new_from_netlink(mUserDevices, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(mDeviceMonitor, "usb", nullptr);
	udev_monitor_enable_receiving(mDeviceMonitor);

	// Get the file descriptor (fd) for the monitor
	mFileDescriptor = udev_monitor_get_fd(mDeviceMonitor);
#endif
}


DeviceListener::~DeviceListener()
{
#if __has_include(<libudev.h>)
	udev_monitor_unref(mDeviceMonitor);
	udev_unref(mUserDevices);
#endif
}
