/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DeviceInfo.h"

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
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
QString DeviceInfo::getField(const char* pField)
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
