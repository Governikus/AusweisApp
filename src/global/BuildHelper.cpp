/*
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BuildHelper.h"

#ifdef Q_OS_ANDROID
#include "VersionNumber.h"

#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

using namespace governikus;

const char* BuildHelper::mDateTime = __DATE__ " / " __TIME__;


#ifdef Q_OS_ANDROID

int BuildHelper::getVersionCode()
{
	if (VersionNumber::getApplicationVersion().isDeveloperVersion())
	{
		return getVersionCode(QStringLiteral("com.governikus.ausweisapp2.dev"));
	}

	return getVersionCode(QStringLiteral("com.governikus.ausweisapp2"));
}


int BuildHelper::getVersionCode(const QString& pPackageName)
{
	static int version_code = -1;

	if (version_code == -1)
	{
		QAndroidJniEnvironment env;

		auto context = QtAndroid::androidContext();
		auto manager = context.callObjectMethod("getPackageManager",
				"()Landroid/content/pm/PackageManager;");

		if (manager.isValid())
		{
			const auto& str = QAndroidJniObject::fromString(pPackageName);
			auto info = manager.callObjectMethod("getPackageInfo",
					"(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;",
					str.object<jstring>(), 0);

			if (info.isValid())
			{
				version_code = info.getField<jint>("versionCode");
			}
		}


		if (env->ExceptionCheck())
		{
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}

	return version_code;
}


#endif
