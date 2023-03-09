/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QTimerEvent>

Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


WifiInfo::WifiInfo()
	: QObject()
	, mWifiEnabled(getCurrentWifiEnabled())
{
	qCDebug(network) << "NOT IMPLEMENTED";
}


bool WifiInfo::getCurrentWifiEnabled()
{
	return true;
}


bool WifiInfo::isWifiEnabled()
{
	return mWifiEnabled;
}
