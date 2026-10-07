/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "BuildHelper.h"

#include "DeviceInfo.h"

#ifdef Q_OS_ANDROID
	#include "VersionNumber.h"

	#include <QCryptographicHash>
	#include <QJniEnvironment>
#endif

#include <QSysInfo>
#include <openssl/crypto.h>

#ifdef Q_OS_WIN
	#include <windows.h>
#endif

using namespace governikus;

#ifdef Q_OS_ANDROID
QJniObject BuildHelper::getPackageInfo(const QString& pPackageName, int pFlags)
{
	QJniEnvironment env;
	QJniObject result;

	if (QJniObject context = QNativeInterface::QAndroidApplication::context(); context.isValid())
	{
		if (auto manager = context.callObjectMethod("getPackageManager", "()Landroid/content/pm/PackageManager;"); manager.isValid())
		{
			const auto& str = QJniObject::fromString(pPackageName);
			result = manager.callObjectMethod("getPackageInfo",
					"(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;",
					str.object<jstring>(), pFlags);
		}
	}

	env.checkAndClearExceptions();

	return result;
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
	const auto info = BuildHelper::getPackageInfo(pPackageName);

	if (info.isValid())
	{
		return info.getField<jint>("versionCode");
	}

	return -1;
}


QString BuildHelper::getPackageName()
{
	if (QJniObject context = QNativeInterface::QAndroidApplication::context(); context.isValid())
	{
		if (auto name = context.callObjectMethod("getPackageName", "()Ljava/lang/String;"); name.isValid())
		{
			return name.toString();
		}
	}

	return QString();
}


QByteArrayList BuildHelper::getAppCertificates(const QString& pPackageName)
{
	const int flags = 0x00000040; // GET_SIGNATURES
	const auto info = BuildHelper::getPackageInfo(pPackageName, flags);

	if (!info.isValid())
	{
		return QByteArrayList();
	}

	const auto signatures = info.getObjectField("signatures", "[Landroid/content/pm/Signature;");
	if (!signatures.isValid())
	{
		return QByteArrayList();
	}

	QJniEnvironment env;
	jobjectArray obj = signatures.object<jobjectArray>();
	const jsize elementCount = env->GetArrayLength(obj);
	QByteArrayList list;
	list.reserve(elementCount);

	for (jsize i = 0; i < elementCount; ++i)
	{
		QJniObject elem = env->GetObjectArrayElement(obj, i);
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
		list << QByteArray(reinterpret_cast<const char* const>(buffer), size);
	}

	env.checkAndClearExceptions();

	return list;
}


#endif

QList<std::pair<QLatin1String, QString>> BuildHelper::getInformationHeader()
{
	QList<std::pair<QLatin1String, QString>> data;
	const auto& add = [&data](const char* pKey, const QString& pStr)
			{
				data << std::make_pair(QLatin1String(pKey), pStr);
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
	add(QT_TR_NOOP("Device"), DeviceInfo::getDeviceName());
#endif

	add(QT_TR_NOOP("Qt Version"), QString::fromLatin1(qVersion()));
	add(QT_TR_NOOP("OpenSSL Version"), QString::fromLatin1(OpenSSL_version(OPENSSL_VERSION)));

	return data;
}


bool BuildHelper::fetchUserInteractive()
{
	bool isInteractive = true;

#ifdef Q_OS_WIN
	HWINSTA station = GetProcessWindowStation();
	if (station != nullptr)
	{
		USEROBJECTFLAGS flags;
		if (GetUserObjectInformation(station, UOI_FLAGS, &flags, sizeof(USEROBJECTFLAGS), nullptr)
				&& ((flags.dwFlags & WSF_VISIBLE) == 0))
		{
			isInteractive = false;
		}
	}
#endif

	return isInteractive;
}


bool BuildHelper::isUserInteractive()
{
	static const bool isInteractive = fetchUserInteractive();
	return isInteractive;
}


void BuildHelper::processInformationHeader(const std::function<void(const QString&, const QString&)>& pFunc, bool pTranslate)
{
	const auto& info = getInformationHeader();

	for (const auto& [key, value] : info)
	{
		pFunc(pTranslate ? tr(key.data()) : QString(key), value);
	}
}


#include "moc_BuildHelper.cpp"
