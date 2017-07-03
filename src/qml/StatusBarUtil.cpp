/*!
 * \brief Utility for changing the color of the status bar.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */
#include "StatusBarUtil.h"

#include <QDebug>
#include <QString>
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QtAndroid>
#endif
using namespace governikus;

void StatusBarUtil::setStatusBarColor(const QString& pColor)
{
#ifdef Q_OS_ANDROID
	QAndroidJniEnvironment env;
	QAndroidJniObject javaActivity(QtAndroid::androidActivity());
	if (env->ExceptionCheck())
	{
		env->ExceptionClear();
		qWarning() << "Exception occurred while acquiring activity";
		return;
	}

	if (javaActivity == nullptr)
	{
		qWarning() << "Failed to acquire activity";
		return;
	}
	QAndroidJniObject jColorName = QAndroidJniObject::fromString(pColor);
	QAndroidJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/StatusBarUtil",
			"setStatusBarColor",
			"(Landroid/app/Activity;Ljava/lang/String;)V",
			javaActivity.object<jobject>(),
			jColorName.object<jstring>());
	if (env->ExceptionCheck())
	{
		env->ExceptionClear();
		qWarning() << "Exception occurred while setting color";
		return;
	}
#else
	Q_UNUSED(pColor)
#endif
}
