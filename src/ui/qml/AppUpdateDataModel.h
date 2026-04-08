/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "GlobalStatus.h"

#include <QDateTime>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class test_AppUpdateDataModel;

namespace governikus
{
class AppUpdateDataModel
	: public QObject
{
	Q_OBJECT
	QML_UNCREATABLE("Used by SettingsModel only")
	QML_ELEMENT
	friend class Env;
	friend class ::test_AppUpdateDataModel;

	Q_PROPERTY(bool updateAvailable READ isUpdateAvailable NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(bool valid READ isValid NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(bool compatible READ isCompatible NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(bool appcastRunning READ isAppcastRunning NOTIFY fireDownloadProgressChanged)
	Q_PROPERTY(bool appcastFailed READ getAppcastFailed NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(bool downloadRunning READ isDownloadRunning NOTIFY fireDownloadProgressChanged)
	Q_PROPERTY(int appcastProgress READ getAppcastProgress NOTIFY fireDownloadProgressChanged)
	Q_PROPERTY(int appcastTotal READ getAppcastTotal NOTIFY fireDownloadProgressChanged)
	Q_PROPERTY(QString appcastUpdateText READ getAppcastUpdateText NOTIFY fireAppcastUpdateTextChanged)
	Q_PROPERTY(QString appcastNoUpdateText READ getAppcastNoUpdateText NOTIFY fireAppcastNoUpdateTextChanged)
	Q_PROPERTY(QString appcastErrorText READ getAppcastErrorText NOTIFY fireAppcastErrorTextChanged)
	Q_PROPERTY(int downloadProgress READ getDownloadProgress NOTIFY fireDownloadProgressChanged)
	Q_PROPERTY(int downloadTotal READ getDownloadTotal NOTIFY fireDownloadProgressChanged)

	Q_PROPERTY(QDateTime date READ getDate NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QString version READ getVersion NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl url READ getUrl NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(int size READ getSize NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl checksumUrl READ getChecksumUrl NOTIFY fireAppUpdateDataChanged)

	private:
		bool mAppcastFinished;
		bool mUpdateAvailable;
		bool mMissingPlatform;
		int mDownloadProgress;
		int mDownloadTotal;
		int mAppcastProgress;
		int mAppcastTotal;

		QString errorFromStatusCode(GlobalStatus::Code pCode) const;
		QString supportInfoFromStatusCode(GlobalStatus::Code pCode) const;
		[[nodiscard]] QString getDownloadFolder() const;

		[[nodiscard]] QDateTime lastAppcastDate() const;
		void setLastAppcastDate(const QDateTime& pDate);

		[[nodiscard]] QString lastAppcastVersion() const;
		void setLastAppcastVersion(const QString& pVersion);
		[[nodiscard]] bool lastAppcastUpdateAvailable() const;

#ifndef QT_NO_DEBUG

	public:
#endif
		AppUpdateDataModel();
		~AppUpdateDataModel() override = default;

	private Q_SLOTS:
		void onAppcastFinished(bool pUpdateAvailable, const GlobalStatus& pStatus);
		void onAppcastProgress(qint64 pBytesReceived, qint64 pBytesTotal);
		void onAppDownloadProgress(qint64 pBytesReceived, qint64 pBytesTotal);
		void onAppDownloadFinished(const GlobalStatus& pError);

	public:
		[[nodiscard]] bool isUpdateAvailable() const;
		[[nodiscard]] bool isValid() const;
		[[nodiscard]] bool isCompatible() const;
		[[nodiscard]] bool isAppcastRunning() const;
		[[nodiscard]] bool getAppcastFailed() const;
		[[nodiscard]] bool isDownloadRunning() const;
		[[nodiscard]] int getAppcastProgress() const;
		[[nodiscard]] int getAppcastTotal() const;
		[[nodiscard]] QString getAppcastUpdateText() const;
		[[nodiscard]] QString getAppcastNoUpdateText() const;
		[[nodiscard]] QString getAppcastErrorText() const;
		[[nodiscard]] int getDownloadProgress() const;
		[[nodiscard]] int getDownloadTotal() const;
		[[nodiscard]] const QDateTime& getDate() const;
		[[nodiscard]] QString getVersion() const;
		[[nodiscard]] const QUrl& getUrl() const;
		[[nodiscard]] int getSize() const;
		[[nodiscard]] const QUrl& getChecksumUrl() const;
		[[nodiscard]] Q_INVOKABLE bool download();
		[[nodiscard]] Q_INVOKABLE bool abortDownload();

	Q_SIGNALS:
		void fireAppUpdateDataChanged();
		void fireDownloadProgressChanged();
		void fireAppUpdateAborted();
		void fireAppUpdateFailed(const QString& pError, const QString& pSupportInfo);
		void fireAppDownloadFinished();
		void fireAppcastUpdateTextChanged();
		void fireAppcastNoUpdateTextChanged();
		void fireAppcastErrorTextChanged();
};

} // namespace governikus
