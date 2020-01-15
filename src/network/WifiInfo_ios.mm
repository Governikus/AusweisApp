/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QLoggingCategory>
#include <QTimerEvent>

#import <Foundation/Foundation.h>
#import <ifaddrs.h>
#import <net/if.h>
#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


WifiInfo::WifiInfo()
	: QObject()
	, mWifiEnabled(getCurrentWifiEnabled())
{
	const int timeoutMs = 1000;
	mWifiCheckTimerId = startTimer(timeoutMs);
}


bool WifiInfo::getCurrentWifiEnabled()
{
	struct ifaddrs* interfaces;
	if (!getifaddrs(&interfaces))
	{

		int en0Count = 0;
		int awdl0Count = 0;

		for (struct ifaddrs* interface = interfaces; interface != nullptr; interface = interface->ifa_next)
		{
			if (!(interface->ifa_flags & IFF_UP))
			{
				continue;
			}

			if (std::strcmp(interface->ifa_name, "en0") == 0)
			{
				++en0Count;
			}
			else if (std::strcmp(interface->ifa_name, "awdl0") == 0)
			{
				++awdl0Count;
			}
		}

		freeifaddrs(interfaces);

		// iOS doesn't provide an api to determine if wifi is enabled/connected
		// On iOS 13 and hopefully later, we can, however, use the network interfaces to
		// infer this in an undocumented hacky way:
		// If there is more than one interface with the name "awdl0" it means wifi is enabled.
		// If there is more than one interface with the name "en0" it means wifi is connected.
		if (awdl0Count > 1 || en0Count > 1)   // Enabled or connected
		{
			return true;
		}
	}

	return false;
}


void WifiInfo::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mWifiCheckTimerId)
	{
		pEvent->accept();

		const bool currentEnabled = getCurrentWifiEnabled() || hasPrivateIpAddress();
		if (mWifiEnabled != currentEnabled)
		{
			mWifiEnabled = currentEnabled;
			Q_EMIT fireWifiEnabledChanged(mWifiEnabled);
		}
	}

	QObject::timerEvent(pEvent);
}


bool WifiInfo::isWifiEnabled()
{
	return mWifiEnabled;
}


#include "moc_WifiInfo.cpp"
