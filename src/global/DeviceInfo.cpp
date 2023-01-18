/*!
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "DeviceInfo.h"

#if defined(Q_OS_ANDROID)
	#include <QJniObject>
#elif defined(Q_OS_IOS)
	#include <sys/sysctl.h>
	#include <sys/utsname.h>
#endif

#include <QDebug>

using namespace governikus;

#if defined(Q_OS_ANDROID)
QString DeviceInfo::getField(const char* const pField)
{
	QJniObject field = QJniObject::getStaticObjectField("android/os/Build", pField, "Ljava/lang/String;");
	if (field == nullptr || !field.isValid())
	{
		qCritical() << "Cannot get field:" << pField;
		return QString();
	}

	return field.toString();
}


#endif


#if defined(Q_OS_IOS)
QString DeviceInfo::getMachineId()
{
	struct utsname systemInfo;
	uname(&systemInfo);

	return QString::fromUtf8(systemInfo.machine);
}


QString DeviceInfo::getBuildNumber()
{
	int mib[] = {CTL_KERN, KERN_OSVERSION};
	u_int namelen = sizeof(mib) / sizeof(mib[0]);

	size_t bufferSize = 0;
	sysctl(mib, namelen, nullptr, &bufferSize, nullptr, 0);

	QByteArray buffer(bufferSize, '\0');
	if (int error = sysctl(mib, namelen, buffer.data(), &bufferSize, nullptr, 0); error)
	{
		qDebug() << "Error trying to retrieve iOS build number:" << strerror(errno);
		return QString();
	}

	return QString::fromLatin1(buffer.data(), buffer.size() - 1);
}


#endif

QString DeviceInfo::getPrettyInfo()
{
	return QStringLiteral("%1 (%2)").arg(getName(), getFingerprint());
}


QString DeviceInfo::getName()
{
#if defined(Q_OS_ANDROID)
	return getField("MODEL");

#elif defined(Q_OS_IOS)
	return getMachineId();

#else
	return QSysInfo::machineHostName();

#endif
}


QString DeviceInfo::getFingerprint()
{
#if defined(Q_OS_ANDROID)
	return getField("FINGERPRINT");

#else
	return QString();

#endif
}


QString DeviceInfo::getOSBuildNumber()
{
#if defined(Q_OS_ANDROID)
	return getField("DISPLAY");

#elif defined(Q_OS_IOS)
	return getBuildNumber();

#else
	return QString();

#endif
}


QString DeviceInfo::getOSVersion()
{
	return QSysInfo::productVersion();
}


QString DeviceInfo::getKernelVersion()
{
	return QSysInfo::kernelVersion();
}


QString DeviceInfo::getVendor()
{
#if defined(Q_OS_ANDROID)
	return getField("MANUFACTURER");

#elif defined(Q_OS_IOS)
	return QStringLiteral("Apple");

#else
	return QString();

#endif
}


QString DeviceInfo::getModelNumber()
{
#if defined(Q_OS_ANDROID)
	return getField("MODEL");

#elif defined(Q_OS_IOS)
	return getMachineId();

#else
	return QString();

#endif
}


QString DeviceInfo::getModelName()
{
#if defined(Q_OS_ANDROID)
	return getField("PRODUCT");

#elif defined(Q_OS_IOS)
	return getMachineId();

#else
	return QString();

#endif
}
