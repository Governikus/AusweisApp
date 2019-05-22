/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QCoreApplication>
#include <QLoggingCategory>
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
#ifndef QT_NO_DEBUG
	if (!QCoreApplication::applicationName().startsWith(QLatin1String("QmlTest")))

#endif
	{
		qCWarning(qml) << "NOT IMPLEMENTED";
	}
}


void WifiInfo::timerEvent(QTimerEvent* pEvent)
{
	QObject::timerEvent(pEvent);
}


bool WifiInfo::isWifiEnabled()
{
#ifndef QT_NO_DEBUG
	if (!QCoreApplication::applicationName().startsWith(QLatin1String("QmlTest")))

#endif
	{
		qCWarning(qml) << "NOT IMPLEMENTED";
	}

	return mWifiEnabled;
}


#include "moc_WifiInfo.cpp"
