/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdateDataModel.h"

#include "AppSettings.h"
#include "AppUpdater.h"
#include "LanguageLoader.h"
#include "SettingsModel.h"

#include <QDir>
#include <QLoggingCategory>
#include <QProcess>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(update)


AppUpdateDataModel::AppUpdateDataModel()
	: QObject()
	, mAppcastFinished(false)
	, mUpdateAvailable(false)
	, mMissingPlatform(false)
	, mDownloadProgress(0)
	, mDownloadTotal(0)
	, mAppcastProgress(0)
	, mAppcastTotal(0)
{
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppcastCheckFinished, this, &AppUpdateDataModel::onAppcastFinished);
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppcastProgress, this, &AppUpdateDataModel::onAppcastProgress);
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppDownloadProgress, this, &AppUpdateDataModel::onAppDownloadProgress);
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppDownloadFinished, this, &AppUpdateDataModel::onAppDownloadFinished);
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireDownloadTypeChanged, this, &AppUpdateDataModel::fireDownloadProgressChanged);

	connect(this, &AppUpdateDataModel::fireAppUpdateDataChanged, this, &AppUpdateDataModel::fireAppcastUpdateTextChanged);
	connect(this, &AppUpdateDataModel::fireAppUpdateDataChanged, this, &AppUpdateDataModel::fireAppcastErrorTextChanged);
}


QString AppUpdateDataModel::errorFromStatusCode(GlobalStatus::Code pCode) const
{
	switch (pCode)
	{
		case GlobalStatus::Code::Downloader_Aborted:
			return QString();

		case GlobalStatus::Code::Downloader_Data_Corrupted:
			//: DESKTOP Text of the popup that is shown when the app download failed because of a wrong checksum.
			return tr("The received data is broken. Check your network connection and try to restart the update.");

		case GlobalStatus::Code::Update_Execution_Failed:
		{
			const auto& a_start = QStringLiteral("<a href=\"%1\">").arg(getDownloadFolder());
			const auto& a_end = QStringLiteral("</a>");
			//: DESKTOP Text of the popup that is shown when the execution of the update failed (1/2).
			return tr("The update could not be started automatically after a successful download. Please try to do a manual update. You can find the downloaded file %1here%2.").arg(a_start, a_end);
		}

		default:
			//: DESKTOP Generic text of the popup that is shown when the app download failed.
			return tr("An unknown network error occurred. Check your network connection and try to restart the update.");

	}
}


QString AppUpdateDataModel::supportInfoFromStatusCode(GlobalStatus::Code pCode) const
{
	if (pCode == GlobalStatus::Code::Update_Execution_Failed)
	{
		const auto& language = Env::getSingleton<SettingsModel>()->getLanguage();
		const auto& a_start = QStringLiteral("<a href=\"https://www.ausweisapp.bund.de/%1/aa2/support\">").arg(language);
		const auto& a_end = QStringLiteral("</a>");
		//: DESKTOP Text of the popup that is shown when the execution of the update failed (2/2).
		return tr("If this does not help, contact our %1support%2.").arg(a_start, a_end);
	}
	return QString();
}


void AppUpdateDataModel::onAppcastFinished(bool pUpdateAvailable, const GlobalStatus& pStatus)
{
	mAppcastProgress = pStatus.isError() ? 0 : mAppcastTotal;
	Q_EMIT fireDownloadProgressChanged();

	mAppcastFinished = pStatus.getStatusCode() != GlobalStatus::Code::Downloader_Aborted;
	mUpdateAvailable = pUpdateAvailable;
	if (mAppcastFinished)
	{
		setLastAppcastDate(QDateTime::currentDateTime());
	}
	if (pStatus.getStatusCode() == GlobalStatus::Code::No_Error && isValid())
	{
		setLastAppcastVersion(Env::getSingleton<AppUpdater>()->getUpdateData().getVersion());
	}
	mMissingPlatform = pStatus.getStatusCode() == GlobalStatus::Code::Downloader_Missing_Platform;
	Q_EMIT fireAppUpdateDataChanged();
}


void AppUpdateDataModel::onAppcastProgress(qint64 pBytesReceived, qint64 pBytesTotal)
{
	mAppcastProgress = static_cast<int>(pBytesReceived / 1000);
	mAppcastTotal = static_cast<int>(pBytesTotal / 1000);
	Q_EMIT fireDownloadProgressChanged();
}


void AppUpdateDataModel::onAppDownloadProgress(qint64 pBytesReceived, qint64 pBytesTotal)
{
	mDownloadProgress = static_cast<int>(pBytesReceived / 1000);
	mDownloadTotal = static_cast<int>(pBytesTotal / 1000);
	Q_EMIT fireDownloadProgressChanged();
}


void AppUpdateDataModel::onAppDownloadFinished(const GlobalStatus& pError)
{
	mDownloadTotal = getSize() / 1000;
	if (pError.isError())
	{
		mDownloadProgress = 0;

		Q_EMIT fireDownloadProgressChanged();

		const auto& statusCode = pError.getStatusCode();

		if (statusCode == GlobalStatus::Code::Downloader_Aborted)
		{
			Q_EMIT fireAppUpdateAborted();
			return;
		}

		Q_EMIT fireAppUpdateFailed(errorFromStatusCode(statusCode), supportInfoFromStatusCode(statusCode));
		return;
	}

	mDownloadProgress = mDownloadTotal;
	Q_EMIT fireDownloadProgressChanged();

#if defined(Q_OS_WIN)
	const QString installer = Env::getSingleton<AppUpdater>()->getUpdateData().getUpdatePackagePath();
	qCInfo(update) << "Attempting to start update:" << installer;

	auto arguments = QStringList();
	arguments << QStringLiteral("/passive");
	arguments << QStringLiteral("/i") << installer;
	arguments << QStringLiteral("LAUNCH=true");

	if (QProcess::startDetached(QStringLiteral("msiexec.exe"), arguments))
	{
		qCInfo(update) << "New process successfully launched.";
		Q_EMIT fireAppDownloadFinished();
		return;
	}

	qCCritical(update) << "Could not launch new process.";
#endif

	Q_EMIT fireAppUpdateFailed(errorFromStatusCode(GlobalStatus::Code::Update_Execution_Failed), supportInfoFromStatusCode(GlobalStatus::Code::Update_Execution_Failed));
}


bool AppUpdateDataModel::isUpdateAvailable() const
{
	return mUpdateAvailable || lastAppcastUpdateAvailable();
}


bool AppUpdateDataModel::isValid() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().isValid();
}


bool AppUpdateDataModel::isCompatible() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().isCompatible();
}


int AppUpdateDataModel::getAppcastProgress() const
{
	return mAppcastProgress;
}


int AppUpdateDataModel::getAppcastTotal() const
{
	return mAppcastTotal;
}


QString AppUpdateDataModel::getAppcastUpdateText() const
{
	const auto& version = getVersion();
	if (version.isNull())
	{
		return {};
	}

	//: DESKTOP %1 is replaced with the version number of the software update
	return tr("An update is available (version %1).").arg(version);
}


QString AppUpdateDataModel::getAppcastNoUpdateText() const
{
	const auto& date = lastAppcastDate();
	if (date.isNull())
	{
		return {};
	}

	//: DESKTOP Date format according to https://doc.qt.io/qt/qdate.html#toString
	const auto& dateString = LanguageLoader::getInstance().getUsedLocale().toString(date, tr("dd.MM.yyyy"));

	//: DESKTOP Time format according to https://doc.qt.io/qt/qtime.html#toString
	const auto& timeString = LanguageLoader::getInstance().getUsedLocale().toString(date, tr("hh:mm AP"));

	//: DESKTOP %1 will be replaced with the date and %2 with the time of the last search.
	const auto& lastSearch = tr("Last search on %1 at %2").arg(dateString, timeString);

	//: DESKTOP %1 is replaced with the version number of the software and %2 is replaced with the application name.
	return tr("Your version %1 of %2 is up to date.").arg(QCoreApplication::applicationVersion(), QCoreApplication::applicationName())
		   + QStringLiteral("<br><i>%1</i>").arg(lastSearch);
}


QString AppUpdateDataModel::getAppcastErrorText() const
{
	if (!getAppcastFailed())
	{
		return {};
	}

	if (mMissingPlatform)
	{
		//: DESKTOP
		return tr("An update information for your platform is not available.");
	}

	if (mUpdateAvailable)
	{
		//: DESKTOP The updater found an update but not all required update information are valid, this should be a very rare case.
		return tr("An update is available but retrieving the information failed.");
	}

	//: DESKTOP
	return tr("The update information could not be retrieved. Please check your network connection.");
}


bool AppUpdateDataModel::isAppcastRunning() const
{
	return Env::getSingleton<AppUpdater>()->getDownloadType() == AppUpdater::DownloadType::UPDATEINFO;
}


bool AppUpdateDataModel::getAppcastFailed() const
{
	return mAppcastFinished && !isValid();
}


bool AppUpdateDataModel::isDownloadRunning() const
{
	const auto& downloadType = Env::getSingleton<AppUpdater>()->getDownloadType();
	return downloadType == AppUpdater::DownloadType::CHECKSUM || downloadType == AppUpdater::DownloadType::APPLICATION;
}


int AppUpdateDataModel::getDownloadProgress() const
{
	return mDownloadProgress;
}


int AppUpdateDataModel::getDownloadTotal() const
{
	return mDownloadTotal;
}


QString AppUpdateDataModel::getDownloadFolder() const
{
	const QString updateFile = Env::getSingleton<AppUpdater>()->getUpdateData().getUpdatePackagePath();
	QUrl updateFolderUrl = QUrl::fromLocalFile(updateFile);
	return QDir::toNativeSeparators(updateFolderUrl.adjusted(QUrl::RemoveFilename).toString());
}


const QDateTime& AppUpdateDataModel::getDate() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getDate();
}


QString AppUpdateDataModel::getVersion() const
{
	if (isValid())
	{
		return Env::getSingleton<AppUpdater>()->getUpdateData().getVersion();
	}

	return lastAppcastVersion();
}


const QUrl& AppUpdateDataModel::getUrl() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getUrl();
}


int AppUpdateDataModel::getSize() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getSize();
}


const QUrl& AppUpdateDataModel::getChecksumUrl() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getChecksumUrl();
}


bool AppUpdateDataModel::download()
{
	mDownloadProgress = 0;
	Q_EMIT fireDownloadProgressChanged();
	return Env::getSingleton<AppUpdater>()->downloadUpdate();
}


bool AppUpdateDataModel::abortDownload()
{
	const auto success = Env::getSingleton<AppUpdater>()->abortDownload();
	mDownloadProgress = 0;
	Q_EMIT fireDownloadProgressChanged();
	return success;
}


QDateTime AppUpdateDataModel::lastAppcastDate() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().lastAppcastDate();
}


void AppUpdateDataModel::setLastAppcastDate(const QDateTime& pDate)
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setLastAppcastDate(pDate);
	Q_EMIT fireAppcastNoUpdateTextChanged();
}


QString AppUpdateDataModel::lastAppcastVersion() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().lastAppcastVersion();
}


void AppUpdateDataModel::setLastAppcastVersion(const QString& pVersion)
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setLastAppcastVersion(pVersion);
	Q_EMIT fireAppcastUpdateTextChanged();
}


bool AppUpdateDataModel::lastAppcastUpdateAvailable() const
{
	const auto& versionString = lastAppcastVersion();
	if (versionString.isNull())
	{
		return false;
	}

	const auto& appcastVersion = VersionNumber(versionString);
	return !appcastVersion.getVersionNumber().isNull() && appcastVersion > VersionNumber::getApplicationVersion();
}
