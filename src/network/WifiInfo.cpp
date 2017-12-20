/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QLoggingCategory>
#include <QTimerEvent>
#if defined(Q_OS_ANDROID)
	#include <QAndroidJniEnvironment>
	#include <QAndroidJniObject>
	#include <QtAndroid>
#endif

Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


WifiInfo::WifiInfo()
	: QObject()
	, mWifiEnabled(getCurrentWifiEnabled())
	, mWifiEnabledTimerId(0)
{
#if defined(Q_OS_ANDROID)
	const int timeoutMs = 1000;
	mWifiEnabledTimerId = startTimer(timeoutMs);
#endif
}


WifiInfo::~WifiInfo()
{

}


bool WifiInfo::getCurrentWifiEnabled()
{
#if !defined(Q_OS_ANDROID)
	return true;

#else
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

#endif
}


void WifiInfo::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mWifiEnabledTimerId)
	{
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
#if !defined(Q_OS_ANDROID)
	qCWarning(qml) << "NOT IMPLEMENTED";
#endif

	return mWifiEnabled;
}
