/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdateDataModel.h"

#include "AppUpdater.h"
#include "SettingsModel.h"

#include <QDir>
#include <QLoggingCategory>
#include <QProcess>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(update)


AppUpdateDataModel::AppUpdateDataModel()
	: QObject()
	, mUpdateAvailable(false)
	, mMissingPlatform(false)
	, mDownloadProgress(0)
	, mDownloadTotal(0)
{
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppcastCheckFinished, this, &AppUpdateDataModel::onAppcastFinished);
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppDownloadProgress, this, &AppUpdateDataModel::onAppDownloadProgress);
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppDownloadFinished, this, &AppUpdateDataModel::onAppDownloadFinished);
}


QString AppUpdateDataModel::errorFromStatusCode(GlobalStatus::Code pCode) const
{
	switch (pCode)
	{
		case GlobalStatus::Code::Downloader_Aborted:
			return QString();

		case GlobalStatus::Code::Downloader_Data_Corrupted:
			//: INFO DESKTOP Text of the popup that is shown when the app download failed because of a wrong checksum.
			return tr("The received data is broken. Check your network connection and try to restart the update.");

		case GlobalStatus::Code::Update_Execution_Failed:
		{
			const auto& a_start = QStringLiteral("<a href=\"%1\">").arg(getDownloadFolder());
			const auto& a_end = QStringLiteral("</a>");
			//: INFO DESKTOP Text of the popup that is shown when the execution of the update failed (1/2).
			return tr("The update could not be started automatically after a successful download. Please try to do a manual update. You can find the downloaded file %1here%2.").arg(a_start, a_end);
		}

		default:
			//: INFO DESKTOP Generic text of the popup that is shown when the app download failed.
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
		//: INFO DESKTOP Text of the popup that is shown when the execution of the update failed (2/2).
		return tr("If this does not help, contact our %1support%2.").arg(a_start, a_end);
	}
	return QString();
}


void AppUpdateDataModel::onAppcastFinished(bool pUpdateAvailable, const GlobalStatus& pStatus)
{
	mUpdateAvailable = pUpdateAvailable;
	mMissingPlatform = pStatus.getStatusCode() == GlobalStatus::Code::Downloader_Missing_Platform;
	Q_EMIT fireAppUpdateDataChanged();
}


void AppUpdateDataModel::onAppDownloadProgress(qint64 pBytesReceived, qint64 pBytesTotal)
{
	mDownloadProgress = static_cast<int>(pBytesReceived / 1024);
	mDownloadTotal = static_cast<int>(pBytesTotal / 1024);
	Q_EMIT fireDownloadProgressChanged();
}


void AppUpdateDataModel::onAppDownloadFinished(const GlobalStatus& pError)
{
	mDownloadTotal = getSize() / 1024;
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
	return mUpdateAvailable;
}


bool AppUpdateDataModel::isMissingPlatform() const
{
	return mMissingPlatform;
}


bool AppUpdateDataModel::isValid() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().isValid();
}


bool AppUpdateDataModel::isCompatible() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().isCompatible();
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


const QString& AppUpdateDataModel::getVersion() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getVersion();
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


const QUrl& AppUpdateDataModel::getNotesUrl() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getNotesUrl();
}


const QString& AppUpdateDataModel::getNotes() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getNotes();
}


bool AppUpdateDataModel::download()
{
	mDownloadProgress = 0;
	const auto success = Env::getSingleton<AppUpdater>()->downloadUpdate();
	Q_EMIT fireDownloadProgressChanged();
	return success;
}


bool AppUpdateDataModel::abortDownload()
{
	mDownloadProgress = 0;
	const auto success = Env::getSingleton<AppUpdater>()->abortDownload();
	Q_EMIT fireDownloadProgressChanged();
	return success;
}
