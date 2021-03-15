/*
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "BuildHelper.h"

#include "DeviceInfo.h"

#ifdef Q_OS_ANDROID
#include "VersionNumber.h"

#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

#include <openssl/crypto.h>
#include <QSysInfo>

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
		list << QByteArray(reinterpret_cast<const char* const>(buffer), size).toHex();
	}

	if (env->ExceptionCheck())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}

	return list;
}


#endif

QVector<QPair<QLatin1String, QString> > BuildHelper::getInformationHeader()
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	#define OpenSSL_version SSLeay_version
	#define OPENSSL_VERSION SSLEAY_VERSION
#endif

	QVector<QPair<QLatin1String, QString> > data;
	const auto& add = [&data](const char* pKey, const QString& pStr)
			{
				data << qMakePair(QLatin1String(pKey), pStr);
			};

	add(QT_TR_NOOP("Application"), QCoreApplication::applicationName());
	add(QT_TR_NOOP("Application Version"), QCoreApplication::applicationVersion());
	add(QT_TR_NOOP("Organization"), QCoreApplication::organizationName());
	add(QT_TR_NOOP("Organization Domain"), QCoreApplication::organizationDomain());

	add(QT_TR_NOOP("System"), QSysInfo::prettyProductName());
	add(QT_TR_NOOP("Kernel"), QSysInfo::kernelVersion());

	QString architecture = QSysInfo::currentCpuArchitecture();
#ifdef Q_OS_ANDROID
	if (architecture != QSysInfo::buildCpuArchitecture())
	{
		architecture += QStringLiteral(" (%1)").arg(QSysInfo::buildCpuArchitecture());
	}
#endif
	add(QT_TR_NOOP("Architecture"), architecture);

#ifdef Q_OS_ANDROID
	add(QT_TR_NOOP("Device"), DeviceInfo::getPrettyInfo());
	add(QT_TR_NOOP("VersionCode"), QString::number(getVersionCode()));
#else
	add(QT_TR_NOOP("Device"), DeviceInfo::getName());
#endif

	add(QT_TR_NOOP("Qt Version"), QString::fromLatin1(qVersion()));
	add(QT_TR_NOOP("OpenSSL Version"), QString::fromLatin1(OpenSSL_version(OPENSSL_VERSION)));

	return data;
}


void BuildHelper::processInformationHeader(const std::function<void(const QString&, const QString&)>& pFunc, bool pTranslate)
{
	const auto& info = getInformationHeader();

	for (const auto& entry : info)
	{
		const auto& key = entry.first;
		pFunc(pTranslate ? tr(key.data()) : QString(key), entry.second);
	}
}
