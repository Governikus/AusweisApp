/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "AppUpdateBackend.h"

#include "GeneralSettings.h"
#include "VersionNumber.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(update)


AppUpdateBackend::AppUpdateBackend()
	: UpdateBackend()
	, mNotesService()
	, mUpdateData()
{
	connect(&mNotesService, &UpdateService::fireUpdateFinished, this, &AppUpdateBackend::onReleaseNotesFinished);
}


void AppUpdateBackend::setTrustedUpdateCertificates(const QVector<QSslCertificate>& pTrustedUpdateCertificates)
{
	mNotesService.setTrustedUpdateCertificates(pTrustedUpdateCertificates);
}


QDateTime AppUpdateBackend::getIssueDate() const
{
	return QDateTime(); // do not abort if Last-Modified is older
}


void AppUpdateBackend::processSuccess(const QByteArray& pData)
{
	QJsonParseError jsonError;
	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qCWarning(update) << "Cannot parse json data:" << jsonError.errorString();
		return;
	}

	const auto& obj = json.object();
	const QJsonValue& items = obj["items"];
	if (items.isArray())
	{
		const auto& itemArray = items.toArray();
		const auto& end = itemArray.constEnd();
		for (auto iter = itemArray.constBegin(); iter != end; ++iter)
		{
			if (iter->isObject())
			{
				if (checkPlatformObject(iter->toObject()))
				{
					mNotesService.runUpdate();
					return;
				}
			}
			else
			{
				qCWarning(update) << "Object of field 'items' cannot be parsed";
			}
		}
	}
	else
	{
		qCWarning(update) << "Field 'items' cannot be parsed";
	}

	Q_EMIT fireAppUpdateCheckFinished(false);
}


void AppUpdateBackend::processError(const GlobalStatus& pError)
{
	qCDebug(update) << "Cannot process app update check:" << pError;
	Q_EMIT fireAppUpdateCheckFinished(false, pError);
}


void AppUpdateBackend::onReleaseNotesFinished()
{
	mUpdateData.setNotes(QString::fromUtf8(mNotesService.getContent()));
	Q_EMIT fireAppUpdateCheckFinished(true, mNotesService.getError());
}


void AppUpdateBackend::skipVersion(const QString& pVersion)
{
	qCInfo(update) << "Skip application update:" << pVersion;
	GeneralSettings::skipVersion(pVersion);
}


bool AppUpdateBackend::checkPlatformObject(const QJsonObject& pJson)
{
	const auto& platform = pJson[QLatin1String("platform")].toString();

	if (!isPlatform(platform))
	{
		qCDebug(update) << "Unused platform:" << platform;
		return false;
	}

	qCDebug(update) << "Found platform:" << platform;
	const auto& version = pJson[QLatin1String("version")].toString();

	if (version == GeneralSettings::getSkipVersion())
	{
		qCInfo(update) << "Version will be skipped:" << version;
		return false;
	}

	AppUpdateData data;
	data.setDate(QDateTime::fromString(pJson[QLatin1String("date")].toString(), Qt::ISODate));
	data.setVersion(version);
	data.setUrl(QUrl(pJson[QLatin1String("url")].toString()));
	data.setChecksumUrl(QUrl(pJson[QLatin1String("checksum")].toString()));
	data.setNotesUrl(QUrl(pJson[QLatin1String("notes")].toString()));
	data.setSize(pJson[QLatin1String("size")].toInt(-1));

	if (!data.isValid())
	{
		qCWarning(update) << "Invalid item:" << pJson;
		return false;
	}

	const auto& itemVersion = VersionNumber(version);
	if (itemVersion > VersionNumber::getApplicationVersion())
	{
		qCInfo(update) << "Found new version:" << version;
		mNotesService.setUpdateUrl(data.getNotesUrl());
		mUpdateData = data;
		return true;
	}

	qCDebug(update) << "No new version:" << version;
	return false;
}


const AppUpdateData& AppUpdateBackend::getUpdateData() const
{
	return mUpdateData;
}


bool AppUpdateBackend::isPlatform(const QString& pPlatform) const
{
#ifdef Q_OS_WIN64
	if (pPlatform == QLatin1String("Q_OS_WIN64"))
	{
		return true;
	}
#endif

#ifdef Q_OS_WIN32
	if (pPlatform == QLatin1String("Q_OS_WIN32"))
	{
		return true;
	}
#endif

#ifdef Q_OS_OSX
	if (pPlatform == QLatin1String("Q_OS_MAC"))
	{
		return true;
	}
#endif

#if !defined(Q_OS_OSX) && !defined(Q_OS_WIN)
	if (pPlatform == QLatin1String("SOURCES"))
	{
		return true;
	}
#endif

	return false;
}
