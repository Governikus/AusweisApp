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


} // namespace


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


QByteArrayList BuildHelper::getAppCertificates()
{
	return getAppCertificates(getPackageName());
}


QByteArrayList BuildHelper::getAppCertificates(const QString& pPackageName)
{
	const int flags = 0x00000040; // GET_SIGNATURES
	const auto info = getPackageInfo(pPackageName, flags);

	if (!info.isValid())
	{
		return QByteArrayList();
	}

	const auto signatures = info.getObjectField("signatures", "[Landroid/content/pm/Signature;");
	if (!signatures.isValid())
	{
		return QByteArrayList();
	}

	QAndroidJniEnvironment env;
	jobjectArray obj = signatures.object<jobjectArray>();
	const jsize elementCount = env->GetArrayLength(obj);
	QByteArrayList list;
	list.reserve(elementCount);

	for (jsize i = 0; i < elementCount; ++i)
	{
		QAndroidJniObject elem = env->GetObjectArrayElement(obj, i);
		if (!elem.isValid())
		{
			continue;
		}

		auto bytes = elem.callObjectMethod("toByteArray", "()[B");
		if (!bytes.isValid())
		{
			continue;
		}

		jbyteArray data = bytes.object<jbyteArray>();
		const auto size = env->GetArrayLength(data);
		jbyte* buffer = env->GetByteArrayElements(data, 0);
		list << QByteArray(reinterpret_cast<const char*>(buffer), size).toHex();
	}

	if (env->ExceptionCheck())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}

	return list;
}


#endif
