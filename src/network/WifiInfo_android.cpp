/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QCoreApplication>
#include <QJniEnvironment>
#include <QJniObject>
#include <QLoggingCategory>
#include <QTimerEvent>

Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


WifiInfo::WifiInfo()
	: QObject()
	, mWifiEnabled(getCurrentWifiEnabled())
{
	const int timeoutMs = 1000;
	mWifiCheckTimerId = startTimer(timeoutMs);
}


bool WifiInfo::getCurrentWifiEnabled() const
{
	QJniEnvironment env;
	const QJniObject context(QNativeInterface::QAndroidApplication::context());
	if (!context.isValid())
	{
		qCCritical(network) << "Cannot determine android context.";
		return false;
	}

	const jboolean jEnabled = QJniObject::callStaticMethod<jboolean>("com/governikus/ausweisapp2/WifiInfo",
			"wifiEnabled",
			"(Landroid/content/Context;)Z",
			context.object<jobject>());

	if (env->ExceptionCheck())
	{
		qCCritical(network) << "Cannot call WifiInfo.wifiEnabled()";
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


bool WifiInfo::isWifiEnabled() const
{
	return mWifiEnabled;
}


#include "moc_WifiInfo.cpp"
