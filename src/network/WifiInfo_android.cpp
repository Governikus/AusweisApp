/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QLoggingCategory>
#include <QtAndroid>
#include <QTimerEvent>

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
	QAndroidJniEnvironment env;
	const QAndroidJniObject context(QtAndroid::androidContext());
	if (!context.isValid())
	{
		qCCritical(qml) << "Cannot determine android context.";
		return false;
	}

	const jboolean jEnabled = QAndroidJniObject::callStaticMethod<jboolean>("com/governikus/ausweisapp2/WifiInfo",
			"wifiEnabled",
			"(Landroid/content/Context;)Z",
			context.object<jobject>());

	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Cannot call WifiInfo.wifiEnabled()";
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}

	return jEnabled == JNI_TRUE;
}


void WifiInfo::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mWifiCheckTimerId)
	{
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
