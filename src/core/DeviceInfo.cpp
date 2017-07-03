/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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
#ifdef Q_OS_ANDROID
	return QStringLiteral("%1 (%2)").arg(getModel(), getFingerprint());

#endif

	return QString();
}


QString DeviceInfo::getModel()
{
#ifdef Q_OS_ANDROID
	return getField("MODEL");

#endif

	return QString();
}


QString DeviceInfo::getFingerprint()
{
#ifdef Q_OS_ANDROID
	return getField("FINGERPRINT");

#endif

	return QString();
}
