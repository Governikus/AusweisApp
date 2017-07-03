/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "ShareUtil.h"

#include <QDebug>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QtAndroid>
#include <jni.h>


using namespace governikus;


void ShareUtil::shareText(const QString& pText, const QString& pChooserTitle)
{
	QAndroidJniEnvironment env;
	QAndroidJniObject javaActivity(QtAndroid::androidActivity());
	if (env->ExceptionCheck())
	{
		env->ExceptionClear();
		return;
	}

	if (javaActivity == nullptr)
	{
		return;
	}

	QAndroidJniObject jText = QAndroidJniObject::fromString(pText);
	QAndroidJniObject jTitle = QAndroidJniObject::fromString(pChooserTitle);
	QAndroidJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/ShareUtil",
			"shareText",
			"(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V",
			javaActivity.object<jobject>(),
			jText.object<jstring>(),
			jTitle.object<jstring>());
	if (env->ExceptionCheck())
	{
		env->ExceptionClear();
		return;
	}
}


#include "moc_ShareUtil.cpp"
