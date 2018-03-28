/*
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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
namespace
{
QAndroidJniObject getPackageInfo(const QString& pPackageName, int pFlags = 0)
{
	QAndroidJniEnvironment env;

	auto context = QtAndroid::androidContext();
	auto manager = context.callObjectMethod("getPackageManager",
			"()Landroid/content/pm/PackageManager;");

	if (manager.isValid())
	{
		const auto& str = QAndroidJniObject::fromString(pPackageName);
		return manager.callObjectMethod("getPackageInfo",
				"(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;",
				str.object<jstring>(), pFlags);
	}

	if (env->ExceptionCheck())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}

	return QAndroidJniObject();
}


}


int BuildHelper::getVersionCode()
{
	static int version_code = -1;

	if (version_code == -1)
	{
		version_code = getVersionCode(getPackageName());
	}

	return version_code;
}


int BuildHelper::getVersionCode(const QString& pPackageName)
{
	const auto info = getPackageInfo(pPackageName);

	if (info.isValid())
	{
		return info.getField<jint>("versionCode");
	}

	return -1;
}


QString BuildHelper::getPackageName()
{
	auto context = QtAndroid::androidContext();
	auto name = context.callObjectMethod("getPackageName", "()Ljava/lang/String;");
	if (name.isValid())
	{
		return name.toString();
	}

	return QString();
}


#endif
