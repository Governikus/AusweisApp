/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdateData.h"

#include "AppSettings.h"
#include "VersionNumber.h"

#include <QJsonArray>
#include <QJsonParseError>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(appupdate)

AppUpdateData::AppUpdateData()
	: mDate()
	, mVersion()
	, mUrl()
	, mSize(-1)
	, mChecksumUrl()
	, mNotesUrl()
	, mNotes()
	, mParsingResult(GlobalStatus::Code::No_Error)
{
}


AppUpdateData::AppUpdateData(const GlobalStatus& pParsingResult)
	: mDate()
	, mVersion()
	, mUrl()
	, mSize(-1)
	, mChecksumUrl()
	, mNotesUrl()
	, mNotes()
	, mParsingResult(pParsingResult)
{
}


bool AppUpdateData::isValid() const
{
	// Valid means = required data!
	return !mVersion.isEmpty() &&
		   mUrl.isValid() &&
		   mNotesUrl.isValid();
}


void AppUpdateData::setDate(const QDateTime& pDate)
{
	mDate = pDate;
}


const QDateTime AppUpdateData::getDate() const
{
	return mDate;
}


void AppUpdateData::setVersion(const QString& pVersion)
{
	mVersion = pVersion;
}


const QString& AppUpdateData::getVersion() const
{
	return mVersion;
}


void AppUpdateData::setUrl(const QUrl& pUrl)
{
	mUrl = pUrl;
}


const QUrl& AppUpdateData::getUrl() const
{
	return mUrl;
}


int AppUpdateData::getSize() const
{
	return mSize;
}


void AppUpdateData::setSize(int pSize)
{
	if (pSize < 0)
	{
		mSize = -1;
	}
	else
	{
		mSize = pSize;
	}
}


void AppUpdateData::setChecksumUrl(const QUrl& pChecksumUrl)
{
	mChecksumUrl = pChecksumUrl;
}


const QUrl& AppUpdateData::getChecksumUrl() const
{
	return mChecksumUrl;
}


void AppUpdateData::setNotesUrl(const QUrl& pNotesUrl)
{
	mNotesUrl = pNotesUrl;
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


const GlobalStatus& AppUpdateData::getParsingResult() const
{
	return mParsingResult;
}


AppUpdateData AppUpdateData::parse(const QByteArray& pData)
{
	QJsonParseError jsonError;

	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qCWarning(appupdate) << "Cannot parse json data:" << jsonError.errorString();
		return AppUpdateData(GlobalStatus::Code::Downloader_Data_Corrupted);
	}

	const auto& obj = json.object();
	const QJsonValue& items = obj[QLatin1String("items")];

	if (!items.isArray())
	{
		qCWarning(appupdate) << "Field 'items' cannot be parsed";
		return AppUpdateData(GlobalStatus::Code::Downloader_Data_Corrupted);
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
				AppUpdateData newData;
				newData.setVersion(jsonObject[QLatin1String("version")].toString());
				newData.setUrl(QUrl(jsonObject[QLatin1String("url")].toString()));
				newData.setNotesUrl(QUrl(jsonObject[QLatin1String("notes")].toString()));
				newData.setDate(QDateTime::fromString(jsonObject[QLatin1String("date")].toString(), Qt::ISODate));
				newData.setChecksumUrl(QUrl(jsonObject[QLatin1String("checksum")].toString()));
				newData.setSize(jsonObject[QLatin1String("size")].toInt(-1));
				return newData;
			}
		}
		else
		{
			qCWarning(appupdate) << "Object of field 'items' cannot be parsed";
			return AppUpdateData(GlobalStatus::Code::Downloader_Data_Corrupted);
		}
	}

	qCWarning(appupdate()) << "No matching platform found in update json";
	return AppUpdateData(GlobalStatus::Code::No_Error);
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
