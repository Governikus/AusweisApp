/*!
 * \brief Utility for changing the color of the status bar.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StatusBarUtil.h"

#include <QColor>
#include <QDebug>
#include <QString>
#ifdef Q_OS_ANDROID
	#include <QtAndroidExtras/QAndroidJniEnvironment>
	#include <QtAndroidExtras/QAndroidJniObject>
	#include <QtAndroidExtras/QtAndroid>
	#define FLAG_TRANSLUCENT_STATUS 0x04000000
	#define FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS 0x80000000
	#define HSV_VALUE_REDUCTION (255 * 20 / 100)
#endif


using namespace governikus;


bool StatusBarUtil::catchJavaExceptions() const
{
#ifdef Q_OS_ANDROID
	QAndroidJniEnvironment env;
	if (env->ExceptionCheck())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
		qWarning() << "Exception occurred while setting status bar color";
		return true;
	}
#endif
	return false;
}


void StatusBarUtil::setStatusBarColor(const QString& pColor)
{
#ifdef Q_OS_ANDROID
	if (QtAndroid::androidSdkVersion() < 21)
	{
		return;
	}

	QColor color(pColor);
	int newValue = color.value() >= HSV_VALUE_REDUCTION ? color.value() - HSV_VALUE_REDUCTION : 0;
	color.setHsv(color.hue(), color.saturation(), newValue);

	QtAndroid::runOnAndroidThread([ = ](){
				QAndroidJniObject window = QtAndroid::androidActivity().callObjectMethod("getWindow", "()Landroid/view/Window;");
				if (catchJavaExceptions())
				{
					return;
				}

				window.callMethod<void>("addFlags", "(I)V", FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
				window.callMethod<void>("clearFlags", "(I)V", FLAG_TRANSLUCENT_STATUS);
				window.callMethod<void>("setStatusBarColor", "(I)V", color.rgba());
				catchJavaExceptions();
			});

#else
	Q_UNUSED(pColor)
#endif
}
