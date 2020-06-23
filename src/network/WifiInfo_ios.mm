/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QLoggingCategory>
#include <QTimerEvent>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;

WifiInfo::WifiInfo()
	: QObject()
	, mWifiEnabled(false)
{
	mMonitor = nw_path_monitor_create();
	nw_path_monitor_update_handler_t handler = ^(nw_path_t path){
		mWifiEnabled = nw_path_uses_interface_type(path, nw_interface_type_wifi);

		qCDebug(network) << "WiFi status changed to:" << mWifiEnabled;

		Q_EMIT fireWifiEnabledChanged(mWifiEnabled);
	};

	nw_path_monitor_set_queue(mMonitor, dispatch_get_main_queue());

	nw_path_monitor_set_update_handler(mMonitor, handler);
	nw_path_monitor_start(mMonitor);
}


bool WifiInfo::getCurrentWifiEnabled()
{
	return isWifiEnabled();
}


WifiInfo::~WifiInfo()
{
	nw_path_monitor_cancel(mMonitor);
}


bool WifiInfo::isWifiEnabled()
{
	return mWifiEnabled;
}


#include "moc_WifiInfo.cpp"
