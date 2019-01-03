/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "Survey.h"

#ifndef QT_NO_DEBUG
#include <QCoreApplication>
#endif

#include <QJsonDocument>
#include <QJsonObject>


using namespace governikus;


#define VALUE_NAME(_name, _key)\
	inline QLatin1String _name(){\
		return QLatin1String(_key);\
	}


namespace
{
VALUE_NAME(ROM, "Rom")
VALUE_NAME(BUILD_NUMBER, "BuildNumber")
VALUE_NAME(ANDROID_VERSION, "AndroidVersion")
VALUE_NAME(KERNEL_VERSION, "KernelVersion")
VALUE_NAME(MAXIMUM_NFC_PACKET_LENGTH, "MaximumNfcPacketLength")
VALUE_NAME(VENDOR, "Vendor")
VALUE_NAME(MODEL_NUMBER, "ModelNumber")
VALUE_NAME(MODEL_NAME, "ModelName")
VALUE_NAME(AUSWEISAPP_VERSION_NUMBER, "AusweisAppVersionNumber")
} // namespace


Survey::Survey(const QString& pBuildNumber,
		const QString& pAndroidVersion,
		const QString& pKernelVersion,
		int pMaximumNfcPacketLength,
		const QString& pVendor,
		const QString& pModelNumber,
		const QString& pModelName,
		const QString& pAusweisAppVersionNumber)
	: mNull(false)
	, mBuildNumber(pBuildNumber)
	, mAndroidVersion(pAndroidVersion)
	, mKernelVersion(pKernelVersion)
	, mMaximumNfcPacketLength(pMaximumNfcPacketLength)
	, mVendor(pVendor)
	, mModelNumber(pModelNumber)
	, mModelName(pModelName)
	, mAusweisAppVersionNumber(pAusweisAppVersionNumber)
{
}


Survey::Survey()
	: mNull(true)
	, mBuildNumber()
	, mAndroidVersion()
	, mKernelVersion()
	, mMaximumNfcPacketLength(0)
	, mVendor()
	, mModelNumber()
	, mModelName()
	, mAusweisAppVersionNumber()
{
}


QByteArray Survey::toJsonByteArray() const
{
	QJsonObject rom;
	rom[BUILD_NUMBER()] = mBuildNumber;
	rom[ANDROID_VERSION()] = mAndroidVersion;
	rom[KERNEL_VERSION()] = mKernelVersion;
	rom[MAXIMUM_NFC_PACKET_LENGTH()] = QJsonValue(mMaximumNfcPacketLength);

	QJsonObject mainObject;
	mainObject[ROM()] = rom;
	mainObject[VENDOR()] = mVendor;
	mainObject[MODEL_NUMBER()] = mModelNumber;
	mainObject[MODEL_NAME()] = mModelName;
	mainObject[AUSWEISAPP_VERSION_NUMBER()] = mAusweisAppVersionNumber;

#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		return QJsonDocument(mainObject).toJson(QJsonDocument::Indented);
	}
#endif

	return QJsonDocument(mainObject).toJson(QJsonDocument::Compact);
}


bool Survey::isNull() const
{
	return mNull;
}
