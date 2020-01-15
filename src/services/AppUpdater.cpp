/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdater.h"

#include "AppSettings.h"
#include "Downloader.h"
#include "ProviderConfiguration.h"
#include "SecureStorage.h"
#include "SingletonHelper.h"
#include "VersionNumber.h"

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#include "ReaderConfiguration.h"
#endif

#include <QLoggingCategory>

using namespace governikus;

defineSingleton(AppUpdater)

Q_DECLARE_LOGGING_CATEGORY(appupdate)

AppUpdater::AppUpdater()
	: mIgnoreNextVersionskip(false)
	, mAppUpdateJsonUrl()
	, mAppUpdateData()
{
	const auto* secureStorage = Env::getSingleton<SecureStorage>();

	mAppUpdateJsonUrl = VersionNumber::getApplicationVersion().isDeveloperVersion() ? secureStorage->getAppcastBetaUpdateUrl() : secureStorage->getAppcastUpdateUrl();
}


AppUpdater& AppUpdater::getInstance()
{
	return *Instance;
}


void AppUpdater::checkAppUpdate(bool pIgnoreNextVersionskip)
{
	mIgnoreNextVersionskip = pIgnoreNextVersionskip;
	mAppUpdateData = AppUpdateData();

	auto* downloader = Env::getSingleton<Downloader>();
	connect(downloader, &Downloader::fireDownloadSuccess, this, &AppUpdater::onUpdateDownloadFinished);
	connect(downloader, &Downloader::fireDownloadFailed, this, &AppUpdater::onUpdateDownloadFailed);
	connect(downloader, &Downloader::fireDownloadUnnecessary, this, &AppUpdater::onUpdateDownloadUnnecessary);

	downloader->download(mAppUpdateJsonUrl);
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


void AppUpdater::onUpdateDownloadFinished(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData)
{
	Q_UNUSED(pNewTimestamp)
	if (pUpdateUrl == mAppUpdateJsonUrl)
	{
		AppUpdateData newData = AppUpdateData::parse(pData);
		if (newData.isValid())
		{
			mAppUpdateData = newData;
			const auto& version = mAppUpdateData.getVersion();

			if (VersionNumber(version) > VersionNumber::getApplicationVersion())
			{
				if (!mIgnoreNextVersionskip && version == Env::getSingleton<AppSettings>()->getGeneralSettings().getSkipVersion())
				{
					qCInfo(appupdate) << "Version will be skipped:" << version;
					Q_EMIT fireAppUpdateCheckFinished(false, GlobalStatus::Code::No_Error);
				}
				else
				{
					mIgnoreNextVersionskip = false;
					qCInfo(appupdate) << "Found new version:" << version << ", greater than old version" << QCoreApplication::applicationVersion();
					Env::getSingleton<Downloader>()->download(mAppUpdateData.getNotesUrl());
					return;
				}
			}
			else
			{
				qCDebug(appupdate) << "No new version:" << version;
				Q_EMIT fireAppUpdateCheckFinished(false, GlobalStatus::Code::No_Error);
			}
		}
		else
		{
			Q_EMIT fireAppUpdateCheckFinished(false, newData.getParsingResult().getStatusCode());
		}
		clearDownloaderConnection();
	}
	if (pUpdateUrl == mAppUpdateData.getNotesUrl())
	{
		qCDebug(appupdate) << "Release notes downloaded successfully";
		mAppUpdateData.setNotes(QString::fromUtf8(pData));
		Q_EMIT fireAppUpdateCheckFinished(true, GlobalStatus::Code::No_Error);
		clearDownloaderConnection();
	}
}


void AppUpdater::onUpdateDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode)
{
	if (pUpdateUrl == mAppUpdateJsonUrl)
	{
		qCDebug(appupdate) << "App Update JSON failed:" << GlobalStatus(pErrorCode).toErrorDescription();
		Q_EMIT fireAppUpdateCheckFinished(false, pErrorCode);
		clearDownloaderConnection();
		return;
	}
	if (mAppUpdateData.isValid() && pUpdateUrl == mAppUpdateData.getNotesUrl())
	{
		qCDebug(appupdate) << "Release notes download failed:" << GlobalStatus(pErrorCode).toErrorDescription();
		Q_EMIT fireAppUpdateCheckFinished(true, GlobalStatus::Code::No_Error);
		clearDownloaderConnection();
	}
}


void AppUpdater::onUpdateDownloadUnnecessary(const QUrl& pUpdateUrl)
{
	if (pUpdateUrl == mAppUpdateJsonUrl || pUpdateUrl == mAppUpdateData.getNotesUrl())
	{
		qCCritical(appupdate) << "Got a DownloadUnnecessary from Downloader, but App Updates always have to be fresh, this should not be happening";
		Q_EMIT fireAppUpdateCheckFinished(false, GlobalStatus::Code::Network_Other_Error);
		clearDownloaderConnection();
	}
}


void AppUpdater::clearDownloaderConnection()
{
	auto* const downloader = Env::getSingleton<Downloader>();
	disconnect(downloader, &Downloader::fireDownloadSuccess, this, &AppUpdater::onUpdateDownloadFinished);
	disconnect(downloader, &Downloader::fireDownloadFailed, this, &AppUpdater::onUpdateDownloadFailed);
	disconnect(downloader, &Downloader::fireDownloadUnnecessary, this, &AppUpdater::onUpdateDownloadUnnecessary);
}
