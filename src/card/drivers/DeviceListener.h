/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#if __has_include(<libudev.h>)
	#include <libudev.h>
#endif

#include <QThread>

namespace governikus
{

class DeviceListener
	: public QThread
{
	Q_OBJECT

	private:
#if __has_include(<libudev.h>)
		struct udev* mUserDevices;
		struct udev_monitor* mDeviceMonitor;
		int mFileDescriptor;
#endif

		void run() override;

	public:
		DeviceListener();
		~DeviceListener() override;

	Q_SIGNALS:
		void fireDeviceChangeDetected();
};


} // namespace governikus
