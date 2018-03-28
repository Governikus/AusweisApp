/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QLoggingCategory>
#include <QTimerEvent>

#import <Foundation/Foundation.h>
#import <SystemConfiguration/CaptiveNetwork.h>
#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


WifiInfo::WifiInfo()
	: QObject()
	, mWifiEnabled(getCurrentWifiEnabled())
	, mWifiEnableWaitCounter(0)
{
	const int timeoutMs = 1000;
	mWifiCheckTimerId = startTimer(timeoutMs);
}


bool WifiInfo::getCurrentWifiEnabled()
{
	// https://stackoverflow.com/a/25963050
	Boolean networkInterfaceAvailable = false;
	NSArray* interfaces = CFBridgingRelease(CNCopySupportedInterfaces());
	for (NSString* interface in interfaces)
	{
		NSDictionary* networkInfo = CFBridgingRelease(CNCopyCurrentNetworkInfo(reinterpret_cast<CFStringRef>(interface)));
		if (networkInfo != nullptr)
		{
			networkInterfaceAvailable = true;
			break;
		}
	}
	return networkInterfaceAvailable;
}


void WifiInfo::enableWifi()
{
	NSArray* urlStrings = @[@"prefs:root=WIFI", @"App-Prefs:root=WIFI"];
	for (NSString* urlString in urlStrings)
	{
		NSURL* url = [NSURL URLWithString:urlString];
		if ([[UIApplication sharedApplication] canOpenURL:url])
		{
			[[UIApplication sharedApplication] openURL:url options:@{}
			completionHandler:nil];
			break;
		}
	}
}


void WifiInfo::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mWifiCheckTimerId)
	{
		pEvent->accept();
		const bool currentEnabled = getCurrentWifiEnabled();
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
