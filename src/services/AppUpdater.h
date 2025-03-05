/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppUpdateData.h"
#include "Env.h"
#include "GlobalStatus.h"

#include <QDateTime>
#include <QUrl>

class test_AppUpdatr;

namespace governikus
{
class AppUpdater
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_AppUpdatr;

	private:
		QUrl mAppUpdateJsonUrl;
		AppUpdateData mAppUpdateData;
		QString mDownloadPath;
		bool mDownloadInProgress;

		static QCryptographicHash::Algorithm getHashAlgo(const QByteArray& pAlgo);

		AppUpdater();
		~AppUpdater() override = default;

		void clearDownloaderConnection();
		bool download(const QUrl& pUrl);
		QString save(const QByteArray& pData, const QString& pFilename) const;

	public:
		bool abortDownload() const;
		bool downloadUpdate();
		bool checkAppUpdate();
		[[nodiscard]] const AppUpdateData& getUpdateData() const;

#ifndef QT_NO_DEBUG
		[[nodiscard]] QString getDownloadPath() const;
		void setDownloadPath(const QString& pPath);
#endif

		void handleVersionInfoDownloadFinished(const QByteArray& pData);
		void handleReleaseNotesDownloadFinished(const QByteArray& pData);
		void handleChecksumDownloadFinished(const QUrl& pUpdateUrl, const QByteArray& pData);
		void handleAppDownloadFinished(const QByteArray& pData);

	private Q_SLOTS:
		void onDownloadFinished(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData);
		void onDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode);
		void onDownloadUnnecessary(const QUrl& pUpdateUrl);
		void onDownloadProgress(const QUrl& pUpdateUrl, qint64 pBytesReceived, qint64 pBytesTotal);

	Q_SIGNALS:
		void fireAppcastCheckFinished(bool pUpdateAvailable, const GlobalStatus& pError);
		void fireAppDownloadFinished(const GlobalStatus& pError);
		void fireAppDownloadProgress(qint64 pBytesReceived, qint64 pBytesTotal);
};

} // namespace governikus
