/*!
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdateData.h"

#include "AppSettings.h"
#include "VersionNumber.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonParseError>
#include <QLoggingCategory>
#include <QOperatingSystemVersion>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(appupdate)

AppUpdateData::AppUpdateData(const GlobalStatus& pParsingResult)
	: mMinOsVersion()
	, mDate()
	, mVersion()
	, mUrl()
	, mSize(-1)
	, mChecksumUrl()
	, mNotesUrl()
	, mNotes()
	, mChecksumAlgorithm(QCryptographicHash::Sha256)
	, mChecksum()
	, mChecksumValid(false)
	, mUpdatePackagePath()
	, mParsingResult(pParsingResult)
{
}


AppUpdateData::AppUpdateData(const QByteArray& pData)
	: AppUpdateData(GlobalStatus::Code::No_Error)
{
	QJsonParseError jsonError;

	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qCWarning(appupdate) << "Cannot parse json data:" << jsonError.errorString();
		mParsingResult = GlobalStatus::Code::Downloader_Data_Corrupted;
		return;
	}

	const auto& obj = json.object();
	const QJsonValue& items = obj[QLatin1String("items")];

	if (!items.isArray())
	{
		qCWarning(appupdate) << "Field 'items' cannot be parsed";
		mParsingResult = GlobalStatus::Code::Downloader_Data_Corrupted;
		return;
	}

	const auto& itemArray = items.toArray();
	const auto& end = itemArray.constEnd();
	for (auto iter = itemArray.constBegin(); iter != end; ++iter)
	{
		if (iter->isObject())
		{
			auto jsonObject = iter->toObject();
			if (checkPlatformObject(jsonObject))
			{
				mMinOsVersion = QVersionNumber::fromString(jsonObject[QLatin1String("minimum_platform")].toString());
				mVersion = jsonObject[QLatin1String("version")].toString();
				mUrl = QUrl(jsonObject[QLatin1String("url")].toString());
				mNotesUrl = QUrl(jsonObject[QLatin1String("notes")].toString());
				mDate = QDateTime::fromString(jsonObject[QLatin1String("date")].toString(), Qt::ISODate);
				mChecksumUrl = QUrl(jsonObject[QLatin1String("checksum")].toString());
				mSize = qMax(-1, jsonObject[QLatin1String("size")].toInt(-1));
				return;
			}
		}
		else
		{
			qCWarning(appupdate) << "Object of field 'items' cannot be parsed";
			mParsingResult = GlobalStatus::Code::Downloader_Data_Corrupted;
			return;
		}
	}

	mParsingResult = GlobalStatus::Code::Downloader_Missing_Platform;
	qCWarning(appupdate) << "No matching platform found in update json";
}


bool AppUpdateData::isValid() const
{
	// Valid means = required data!
	return !mVersion.isEmpty() &&
		   mUrl.isValid() &&
		   !mUrl.fileName().isEmpty() &&
		   mChecksumUrl.isValid() &&
		   !mChecksumUrl.fileName().isEmpty() &&
		   mNotesUrl.isValid();
}


const GlobalStatus& AppUpdateData::getParsingResult() const
{
	return mParsingResult;
}


bool AppUpdateData::isCompatible() const
{
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	const auto osv = QOperatingSystemVersion::current();
	const auto currentVersion = QVersionNumber(osv.majorVersion(), osv.minorVersion(), osv.microVersion());
	return currentVersion >= mMinOsVersion;

#else
	return true;

#endif
}


const QDateTime& AppUpdateData::getDate() const
{
	return mDate;
}


const QString& AppUpdateData::getVersion() const
{
	return mVersion;
}


const QUrl& AppUpdateData::getUrl() const
{
	return mUrl;
}


int AppUpdateData::getSize() const
{
	return mSize;
}


const QUrl& AppUpdateData::getChecksumUrl() const
{
	return mChecksumUrl;
}


const QUrl& AppUpdateData::getNotesUrl() const
{
	return mNotesUrl;
}


void AppUpdateData::setNotes(const QString& pNotes)
{
	mNotes = pNotes;
}


const QString& AppUpdateData::getNotes() const
{
	return mNotes;
}


void AppUpdateData::setChecksum(const QByteArray& pChecksum, QCryptographicHash::Algorithm pAlgorithm)
{
	if (mChecksum == pChecksum && mChecksumAlgorithm == pAlgorithm)
	{
		return;
	}

	mChecksumAlgorithm = pAlgorithm;

	if (pChecksum.isEmpty())
	{
		mChecksum.clear();
		mChecksumValid = false;
		return;
	}

	mChecksum = pChecksum.split(' ').first();
	verifyChecksum();
}


const QByteArray& AppUpdateData::getChecksum() const
{
	return mChecksum;
}


void AppUpdateData::verifyChecksum()
{
	if (mUpdatePackagePath.isEmpty()
			|| mChecksum.isEmpty()
			|| !QFile::exists(mUpdatePackagePath))
	{
		mChecksumValid = false;
		return;
	}

	qCDebug(appupdate) << "Verify checksum with algorithm:" << mChecksumAlgorithm;

	QFile file(mUpdatePackagePath);
	file.open(QFile::ReadOnly);
	QCryptographicHash hasher(mChecksumAlgorithm);
	hasher.addData(&file);
	file.close();

	mChecksumValid = hasher.result().toHex() == mChecksum;
}


bool AppUpdateData::isChecksumValid() const
{
	return mChecksumValid;
}


void AppUpdateData::setUpdatePackagePath(const QString& pFile)
{
	mUpdatePackagePath = pFile;
	verifyChecksum();
}


QString AppUpdateData::getUpdatePackagePath() const
{
	return QDir::toNativeSeparators(mUpdatePackagePath);
}


bool AppUpdateData::checkPlatformObject(const QJsonObject& pJson)
{
	const auto& platform = pJson[QLatin1String("platform")].toString();

	if (!isPlatform(platform))
	{
		qCDebug(appupdate) << "Unused platform:" << platform;
		return false;
	}

	qCDebug(appupdate) << "Found platform:" << platform;
	return true;
}


bool AppUpdateData::isPlatform(const QString& pPlatform)
{
#ifdef Q_OS_WIN
	if (pPlatform == QLatin1String("win"))
	{
		return true;
	}
#endif

#ifdef Q_OS_MACOS
	if (pPlatform == QLatin1String("mac"))
	{
		return true;
	}
#endif

#if !defined(Q_OS_MACOS) && !defined(Q_OS_WIN)
	if (pPlatform == QLatin1String("src"))
	{
		return true;
	}
#endif

	return false;
}
