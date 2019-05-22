/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
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
	, mWifiEnableWaitCounter(0)
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


void WifiInfo::enableWifi()
{
	if (mWifiEnabled)
	{
		return;
	}

	QAndroidJniEnvironment env;
	const QAndroidJniObject context(QtAndroid::androidContext());
	if (!context.isValid())
	{
		qCCritical(qml) << "Cannot determine android context.";
		return;
	}

	const jboolean jSuccess = QAndroidJniObject::callStaticMethod<jboolean>("com/governikus/ausweisapp2/WifiInfo",
			"enableWifi",
			"(Landroid/content/Context;)Z",
			context.object<jobject>());

	qCDebug(qml) << "enableWifi() returned:" << (jSuccess == JNI_TRUE);

	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Cannot call WifiInfo.enableWifi()";
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
	}

	if (jSuccess == JNI_TRUE)
	{
		mWifiEnabled = true;
		mWifiEnableWaitCounter = 3;
		Q_EMIT fireWifiEnabledChanged(true);
	}
}


void WifiInfo::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mWifiCheckTimerId)
	{
		if (shouldWifiEnabledBeCalled())
		{
			const bool currentEnabled = getCurrentWifiEnabled() || hasPrivateIpAddress();
			if (mWifiEnabled != currentEnabled)
			{
				mWifiEnabled = currentEnabled;
				Q_EMIT fireWifiEnabledChanged(mWifiEnabled);
			}
		}
	}

	QObject::timerEvent(pEvent);
}


bool WifiInfo::shouldWifiEnabledBeCalled()
{
	if (mWifiEnableWaitCounter <= 0)
	{
		return true;
	}

	--mWifiEnableWaitCounter;
	return false;
}


bool WifiInfo::isWifiEnabled()
{
	return mWifiEnabled;
}


#include "moc_WifiInfo.cpp"
