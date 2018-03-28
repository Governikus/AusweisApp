/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QLoggingCategory>
#include <QNetworkInterface>
#include <QTimerEvent>

Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


WifiInfo::WifiInfo()
	: QObject()
	, mWifiEnabled(getCurrentWifiEnabled())
	, mWifiCheckTimerId(0)
	, mWifiEnableWaitCounter(0)
{
}


bool WifiInfo::getCurrentWifiEnabled()
{
	return true;
}


void WifiInfo::enableWifi()
{
	qCWarning(qml) << "NOT IMPLEMENTED";
}


void WifiInfo::timerEvent(QTimerEvent* pEvent)
{
	QObject::timerEvent(pEvent);
}


bool WifiInfo::isWifiEnabled()
{
	qCWarning(qml) << "NOT IMPLEMENTED";

	return mWifiEnabled;
}


#include "moc_WifiInfo.cpp"
