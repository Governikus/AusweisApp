/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
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
	: mAppUpdateJsonUrl()
	, mAppUpdateData()
	, mDownloadPath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1Char('/'))
	, mDownloadType(DownloadType::NONE)
{
	const auto* secureStorage = Env::getSingleton<SecureStorage>();

	mAppUpdateJsonUrl = VersionNumber::getApplicationVersion().isBetaVersion() ? secureStorage->getAppcastBetaUpdateUrl() : secureStorage->getAppcastUpdateUrl();
}


bool AppUpdater::checkAppUpdate()
{
	mAppUpdateData = AppUpdateData();
	return download(DownloadType::UPDATEINFO);
}


bool AppUpdater::download(DownloadType pDownloadType)
{
	if (mDownloadType != DownloadType::NONE)
	{
		return false;
	}

	auto* downloader = Env::getSingleton<Downloader>();
	connect(downloader, &Downloader::fireDownloadProgress, this, &AppUpdater::onDownloadProgress);
	connect(downloader, &Downloader::fireDownloadSuccess, this, &AppUpdater::onDownloadFinished);
	connect(downloader, &Downloader::fireDownloadFailed, this, &AppUpdater::onDownloadFailed);
	connect(downloader, &Downloader::fireDownloadUnnecessary, this, &AppUpdater::onDownloadUnnecessary);

	const auto url = urlFromDownloadType(pDownloadType);
	if (!url.isValid())
	{
		clearDownloaderConnection();
		return false;
	}

	setDownloadType(pDownloadType);
	downloader->download(url);
	return true;
}


void AppUpdater::setDownloadType(DownloadType pDownloadType)
{
	if (mDownloadType == pDownloadType)
	{
		return;
	}

	mDownloadType = pDownloadType;
	Q_EMIT fireDownloadTypeChanged();
}


QUrl AppUpdater::urlFromDownloadType(DownloadType pDownloadType) const
{
	switch (pDownloadType)
	{
		case DownloadType::UPDATEINFO:
			return mAppUpdateJsonUrl;

		case DownloadType::CHECKSUM:
			return mAppUpdateData.getChecksumUrl();

		case DownloadType::APPLICATION:
			return mAppUpdateData.getUrl();

		default:
			return QUrl();
	}
}


AppUpdater::DownloadType AppUpdater::getDownloadType() const
{
	return mDownloadType;
}


QString AppUpdater::save(const QByteArray& pData, const QString& pFilename) const
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


bool AppUpdater::abortDownload() const
{
	const auto url = urlFromDownloadType(mDownloadType);
	if (!url.isValid())
	{
		return false;
	}

	return Env::getSingleton<Downloader>()->abort(url);
}


bool AppUpdater::downloadUpdate()
{
	Q_ASSERT(mAppUpdateData.isValid());

	if (mAppUpdateData.isValid())
	{
		return download(DownloadType::CHECKSUM);
	}

	return false;
}


const AppUpdateData& AppUpdater::getUpdateData() const
{
	return mAppUpdateData;
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


void AppUpdater::setAppUpdateJsonUrl(const QUrl& pUrl)
{
	mAppUpdateJsonUrl = pUrl;
}


void AppUpdater::setAppUpdateData(const AppUpdateData& pData)
{
	mAppUpdateData = pData;
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
		handleVersionInfoDownloadFinished(AppUpdateData(pData));
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


void AppUpdater::handleVersionInfoDownloadFinished(const AppUpdateData& pData)
{
	if (pData.isValid())
	{
		mAppUpdateData = pData;
		const auto& version = mAppUpdateData.getVersion();

		if (VersionNumber(version) > VersionNumber::getApplicationVersion())
		{
			qCInfo(appupdate) << "Found new version:" << version << ", greater than old version" << QCoreApplication::applicationVersion();
			Q_EMIT fireAppcastCheckFinished(true, GlobalStatus::Code::No_Error);
		}
		else
		{
			qCDebug(appupdate) << "No new version:" << version;
			Q_EMIT fireAppcastCheckFinished(false, GlobalStatus::Code::No_Error);
		}
	}
	else
	{
		Q_EMIT fireAppcastCheckFinished(false, pData.getParsingResult().getStatusCode());
	}
	clearDownloaderConnection();
}


void AppUpdater::handleChecksumDownloadFinished(const QUrl& pUpdateUrl, const QByteArray& pData)
{
	qCDebug(appupdate) << "Checksum file downloaded successfully:" << pUpdateUrl.fileName();
	save(pData, pUpdateUrl.fileName());
	clearDownloaderConnection();

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
			qCDebug(appupdate) << "Reuse valid package...";
			Q_EMIT fireAppDownloadFinished(GlobalStatus::Code::No_Error);
			return;
		}
		qCDebug(appupdate) << "Checksum of package invalid...";
	}

	qCDebug(appupdate) << "Download package...";
	download(DownloadType::APPLICATION);
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
	if (pUpdateUrl == mAppUpdateJsonUrl || pUpdateUrl == mAppUpdateData.getUrl() || pUpdateUrl == mAppUpdateData.getChecksumUrl())
	{
		Q_ASSERT(false);
		qCCritical(appupdate) << "Got a DownloadUnnecessary from Downloader, but App Updates always have to be fresh, this should not be happening";
		Q_EMIT fireAppcastCheckFinished(false, GlobalStatus::Code::Network_Other_Error);
		clearDownloaderConnection();
	}
}


void AppUpdater::onDownloadProgress(const QUrl& pUpdateUrl, qint64 pBytesReceived, qint64 pBytesTotal)
{
	const qint64 total = pBytesTotal == -1 ? mAppUpdateData.getSize() : pBytesTotal;
	if (pUpdateUrl == mAppUpdateJsonUrl)
	{
		Q_EMIT fireAppcastProgress(pBytesReceived, total);

	}
	else if (pUpdateUrl == mAppUpdateData.getUrl())
	{
		Q_EMIT fireAppDownloadProgress(pBytesReceived, total);
	}
}


void AppUpdater::clearDownloaderConnection()
{
	Env::getSingleton<Downloader>()->disconnect(this);
	setDownloadType(DownloadType::NONE);
}
