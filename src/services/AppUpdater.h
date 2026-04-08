/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
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

	public:
		enum class DownloadType
		{
			NONE,
			UPDATEINFO,
			CHECKSUM,
			APPLICATION
		};

	private:
		QUrl mAppUpdateJsonUrl;
		AppUpdateData mAppUpdateData;
		QString mDownloadPath;
		DownloadType mDownloadType;

		static QCryptographicHash::Algorithm getHashAlgo(const QByteArray& pAlgo);

		AppUpdater();
		~AppUpdater() override = default;

		void clearDownloaderConnection();
		bool download(DownloadType pDownloadType);
		QString save(const QByteArray& pData, const QString& pFilename) const;
		void setDownloadType(DownloadType pDownloadType);
		QUrl urlFromDownloadType(DownloadType pDownloadType) const;

	public:
		bool abortDownload() const;
		bool downloadUpdate();
		bool checkAppUpdate();
		[[nodiscard]] DownloadType getDownloadType() const;
		[[nodiscard]] const AppUpdateData& getUpdateData() const;

#ifndef QT_NO_DEBUG
		[[nodiscard]] QString getDownloadPath() const;
		void setDownloadPath(const QString& pPath);
		void setAppUpdateJsonUrl(const QUrl& pUrl);
		void setAppUpdateData(const AppUpdateData& pData);
#endif

		void handleVersionInfoDownloadFinished(const AppUpdateData& pData);
		void handleChecksumDownloadFinished(const QUrl& pUpdateUrl, const QByteArray& pData);
		void handleAppDownloadFinished(const QByteArray& pData);

	private Q_SLOTS:
		void onDownloadFinished(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData);
		void onDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode);
		void onDownloadUnnecessary(const QUrl& pUpdateUrl);
		void onDownloadProgress(const QUrl& pUpdateUrl, qint64 pBytesReceived, qint64 pBytesTotal);

	Q_SIGNALS:
		void fireAppcastCheckFinished(bool pUpdateAvailable, const GlobalStatus& pError);
		void fireAppcastProgress(qint64 pBytesReceived, qint64 pBytesTotal);
		void fireAppDownloadFinished(const GlobalStatus& pError);
		void fireAppDownloadProgress(qint64 pBytesReceived, qint64 pBytesTotal);
		void fireDownloadTypeChanged();
};

} // namespace governikus
