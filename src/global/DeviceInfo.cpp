/*!
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#include "DeviceInfo.h"

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#elif defined(Q_OS_IOS)
#include <sys/utsname.h>
#endif

#include <QDebug>

using namespace governikus;

DeviceInfo::DeviceInfo()
{
}


DeviceInfo::~DeviceInfo()
{
}


#ifdef Q_OS_ANDROID
QString DeviceInfo::getField(const char* const pField)
{
	QAndroidJniObject field = QAndroidJniObject::getStaticObjectField("android/os/Build", pField, "Ljava/lang/String;");
	if (field == nullptr || !field.isValid())
	{
		qCritical() << "Cannot get field:" << pField;
		return QString();
	}

	return field.toString();
}


#endif

QString DeviceInfo::getPrettyInfo()
{
	return QStringLiteral("%1 (%2)").arg(getName(), getFingerprint());
}


QString DeviceInfo::getName()
{
#ifdef Q_OS_ANDROID
	return getField("MODEL");

#elif defined(Q_OS_IOS)
	struct utsname systemInfo;
	uname(&systemInfo);
	return QString::fromUtf8(systemInfo.machine);

#else
	return QSysInfo::machineHostName();

#endif
}


QString DeviceInfo::getFingerprint()
{
#ifdef Q_OS_ANDROID
	return getField("FINGERPRINT");

#else
	return QString();

#endif
}


QString DeviceInfo::getOSBuildNumber()
{
#ifdef Q_OS_ANDROID
	return getField("DISPLAY");

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
#ifdef Q_OS_ANDROID
	return getField("MANUFACTURER");

#else
	return QString();

#endif
}


QString DeviceInfo::getModelNumber()
{
#ifdef Q_OS_ANDROID
	return getField("MODEL");

#else
	return QString();

#endif
}


QString DeviceInfo::getModelName()
{
#ifdef Q_OS_ANDROID
	return getField("PRODUCT");

#else
	return QString();

#endif
}
