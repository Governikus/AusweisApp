/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdater.h"

#include "AppSettings.h"
#include "Downloader.h"
#include "SecureStorage.h"
#include "VersionNumber.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QLoggingCategory>
#include <QMetaEnum>
#include <QStandardPaths>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(appupdate)

AppUpdater::AppUpdater()
	: mForceUpdate(false)
	, mAppUpdateJsonUrl()
	, mAppUpdateData()
	, mDownloadPath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1Char('/'))
	, mDownloadInProgress(false)
{
	const auto* secureStorage = Env::getSingleton<SecureStorage>();

	mAppUpdateJsonUrl = VersionNumber::getApplicationVersion().isBetaVersion() ? secureStorage->getAppcastBetaUpdateUrl() : secureStorage->getAppcastUpdateUrl();
}


bool AppUpdater::checkAppUpdate(bool pForceUpdate)
{
	mForceUpdate = pForceUpdate;
	mAppUpdateData = AppUpdateData();
	return download(mAppUpdateJsonUrl);
}


bool AppUpdater::download(const QUrl& pUrl)
{
	if (mDownloadInProgress)
	{
		return false;
	}

	auto* downloader = Env::getSingleton<Downloader>();
	connect(downloader, &Downloader::fireDownloadProgress, this, &AppUpdater::onDownloadProgress);
	connect(downloader, &Downloader::fireDownloadSuccess, this, &AppUpdater::onDownloadFinished);
	connect(downloader, &Downloader::fireDownloadFailed, this, &AppUpdater::onDownloadFailed);
	connect(downloader, &Downloader::fireDownloadUnnecessary, this, &AppUpdater::onDownloadUnnecessary);

	mDownloadInProgress = true;
	downloader->download(pUrl);
	return true;
}


QString AppUpdater::save(const QByteArray& pData, const QString& pFilename)
{
	const QDir dir(mDownloadPath);
	if (!dir.exists())
	{
		qCDebug(appupdate) << "Create cache directory:" << dir.mkpath(mDownloadPath);
	}

	QFile file(mDownloadPath + pFilename);

	if (!file.open(QIODevice::WriteOnly))
	{
		qCCritical(appupdate) << "File could not be opened for writing:" << file.fileName();
		return QString();
	}

	if (file.write(pData) != pData.size())
	{
		qCCritical(appupdate) << "Not all data could be written to file:" << file.fileName();
		file.close();
		file.remove();
		return QString();
	}

	qCDebug(appupdate) << "Data written to file:" << file.fileName();
	file.close();
	return file.fileName();
}


bool AppUpdater::abortDownload()
{
	if (mDownloadInProgress)
	{
		return Env::getSingleton<Downloader>()->abort(mAppUpdateData.getUrl());
	}

	return false;
}


bool AppUpdater::downloadUpdate()
{
	Q_ASSERT(mAppUpdateData.isValid());

	if (mAppUpdateData.isValid())
	{
		return download(mAppUpdateData.getChecksumUrl());
	}

	return false;
}


const AppUpdateData& AppUpdater::getUpdateData() const
{
	return mAppUpdateData;
}


void AppUpdater::skipVersion(const QString& pVersion)
{
	qCInfo(appupdate) << "Skip application update:" << pVersion;
	Env::getSingleton<AppSettings>()->getGeneralSettings().skipVersion(pVersion);
}


#ifndef QT_NO_DEBUG
QString AppUpdater::getDownloadPath() const
{
	return mDownloadPath;
}


void AppUpdater::setDownloadPath(const QString& pPath)
{
	mDownloadPath = pPath;

	if (!mDownloadPath.endsWith(QLatin1Char('/')))
	{
		mDownloadPath += QLatin1Char('/');
	}
}


#endif


QCryptographicHash::Algorithm AppUpdater::getHashAlgo(const QByteArray& pAlgo)
{
	const QByteArray algo = pAlgo.isEmpty() ? pAlgo : pAlgo.left(1).toUpper() + pAlgo.mid(1).toLower();
	const auto metatype = QMetaEnum::fromType<QCryptographicHash::Algorithm>();
	bool ok = false;
	const int key = metatype.keyToValue(algo.constData(), &ok);
	return ok ? static_cast<QCryptographicHash::Algorithm>(key) : QCryptographicHash::Algorithm::Sha256;
}


void AppUpdater::onDownloadFinished(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData)
{
	Q_UNUSED(pNewTimestamp)

	if (pUpdateUrl == mAppUpdateJsonUrl)
	{
		handleVersionInfoDownloadFinished(pData);
	}
	else if (pUpdateUrl == mAppUpdateData.getNotesUrl())
	{
		handleReleaseNotesDownloadFinished(pData);
	}
	else if (pUpdateUrl == mAppUpdateData.getChecksumUrl())
	{
		handleChecksumDownloadFinished(pUpdateUrl, pData);
	}
	else if (pUpdateUrl == mAppUpdateData.getUrl())
	{
		handleAppDownloadFinished(pData);
	}
	else
	{
		qCWarning(appupdate) << "Unhandled download result from" << pUpdateUrl;
	}
}


void AppUpdater::handleVersionInfoDownloadFinished(const QByteArray& pData)
{
	AppUpdateData newData(pData);
	if (newData.isValid())
	{
		mAppUpdateData = newData;
		const auto& version = mAppUpdateData.getVersion();

		if (VersionNumber(version) > VersionNumber::getApplicationVersion())
		{
			if (!mForceUpdate && version == Env::getSingleton<AppSettings>()->getGeneralSettings().getSkipVersion())
			{
				qCInfo(appupdate) << "Version will be skipped:" << version;
				Q_EMIT fireAppcastCheckFinished(false, GlobalStatus::Code::No_Error);
			}
			else
			{
				mForceUpdate = false;
				qCInfo(appupdate) << "Found new version:" << version << ", greater than old version" << QCoreApplication::applicationVersion();
				Env::getSingleton<Downloader>()->download(mAppUpdateData.getNotesUrl());
				return;
			}
		}
		else
		{
			qCDebug(appupdate) << "No new version:" << version;
			Q_EMIT fireAppcastCheckFinished(false, GlobalStatus::Code::No_Error);
		}
	}
	else
	{
		Q_EMIT fireAppcastCheckFinished(false, newData.getParsingResult().getStatusCode());
	}
	clearDownloaderConnection();
}


void AppUpdater::handleReleaseNotesDownloadFinished(const QByteArray& pData)
{
	qCDebug(appupdate) << "Release notes downloaded successfully";
	mAppUpdateData.setNotes(QString::fromUtf8(pData));
	Q_EMIT fireAppcastCheckFinished(true, GlobalStatus::Code::No_Error);
	clearDownloaderConnection();
}


void AppUpdater::handleChecksumDownloadFinished(const QUrl& pUpdateUrl, const QByteArray& pData)
{
	qCDebug(appupdate) << "Checksum file downloaded successfully:" << pUpdateUrl.fileName();
	save(pData, pUpdateUrl.fileName());

	const auto extensionSplit = pUpdateUrl.fileName().split(QLatin1Char('.'), Qt::SkipEmptyParts);
	const auto extension = extensionSplit.isEmpty() ? QByteArray() : extensionSplit.last().toLatin1();
	mAppUpdateData.setChecksum(pData, getHashAlgo(extension));

	const auto package = mDownloadPath + mAppUpdateData.getUrl().fileName();
	if (QFile::exists(package))
	{
		qCDebug(appupdate) << "Package already exists:" << package;
		mAppUpdateData.setUpdatePackagePath(package);
		if (mAppUpdateData.isChecksumValid())
		{
			clearDownloaderConnection();
			qCDebug(appupdate) << "Re-use valid package...";
			Q_EMIT fireAppDownloadFinished(GlobalStatus::Code::No_Error);
			return;
		}
		qCDebug(appupdate) << "Checksum of package invalid...";
	}

	qCDebug(appupdate) << "Download package...";
	Env::getSingleton<Downloader>()->download(mAppUpdateData.getUrl());
}


void AppUpdater::handleAppDownloadFinished(const QByteArray& pData)
{
	clearDownloaderConnection();
	const auto filename = mAppUpdateData.getUrl().fileName();
	qCDebug(appupdate) << "Package downloaded successfully:" << filename;
	const auto file = save(pData, filename);

	auto status = GlobalStatus::Code::No_Error;
	if (file.isNull())
	{
		status = GlobalStatus::Code::Downloader_Cannot_Save_File;
	}
	else
	{
		mAppUpdateData.setUpdatePackagePath(file);
		if (!mAppUpdateData.isChecksumValid())
		{
			status = GlobalStatus::Code::Downloader_Data_Corrupted;
		}
	}

	Q_EMIT fireAppDownloadFinished(status);
}


void AppUpdater::onDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode)
{
	if (pUpdateUrl == mAppUpdateJsonUrl)
	{
		qCDebug(appupdate) << "App Update JSON failed:" << GlobalStatus(pErrorCode).toErrorDescription();
		Q_EMIT fireAppcastCheckFinished(false, pErrorCode);
	}
	else if (pUpdateUrl == mAppUpdateData.getNotesUrl())
	{
		qCDebug(appupdate) << "Release notes download failed:" << GlobalStatus(pErrorCode).toErrorDescription();
		Q_EMIT fireAppcastCheckFinished(true, pErrorCode);
	}
	else if (pUpdateUrl == mAppUpdateData.getChecksumUrl() || pUpdateUrl == mAppUpdateData.getUrl())
	{
		qCDebug(appupdate) << "Download failed:" << GlobalStatus(pErrorCode).toErrorDescription() << ',' << pUpdateUrl;
		Q_EMIT fireAppDownloadFinished(pErrorCode);
	}
	else
	{
		// do not clear connection to Downloader
		return;
	}

	clearDownloaderConnection();
}


void AppUpdater::onDownloadUnnecessary(const QUrl& pUpdateUrl)
{
	if (pUpdateUrl == mAppUpdateJsonUrl || pUpdateUrl == mAppUpdateData.getNotesUrl() || pUpdateUrl == mAppUpdateData.getUrl() || pUpdateUrl == mAppUpdateData.getChecksumUrl())
	{
		Q_ASSERT(false);
		qCCritical(appupdate) << "Got a DownloadUnnecessary from Downloader, but App Updates always have to be fresh, this should not be happening";
		Q_EMIT fireAppcastCheckFinished(false, GlobalStatus::Code::Network_Other_Error);
		clearDownloaderConnection();
	}
}


void AppUpdater::onDownloadProgress(const QUrl& pUpdateUrl, qint64 pBytesReceived, qint64 pBytesTotal)
{
	if (pUpdateUrl == mAppUpdateData.getUrl())
	{
		const qint64 total = pBytesTotal == -1 ? mAppUpdateData.getSize() : pBytesTotal;
		Q_EMIT fireAppDownloadProgress(pBytesReceived, total);
	}
}


void AppUpdater::clearDownloaderConnection()
{
	Env::getSingleton<Downloader>()->disconnect(this);
	mDownloadInProgress = false;
}
