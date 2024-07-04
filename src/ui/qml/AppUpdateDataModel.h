/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "GlobalStatus.h"

#include <QDateTime>
#include <QUrl>
#include <QtQml/qqmlregistration.h>


namespace governikus
{
class AppUpdateDataModel
	: public QObject
{
	Q_OBJECT
	QML_UNCREATABLE("Used by SettingsModel only")
	QML_ELEMENT
	friend class Env;

	Q_PROPERTY(bool updateAvailable READ isUpdateAvailable NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(bool missingPlatform READ isMissingPlatform NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(bool valid READ isValid NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(bool compatible READ isCompatible NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(int downloadProgress READ getDownloadProgress NOTIFY fireDownloadProgressChanged)
	Q_PROPERTY(int downloadTotal READ getDownloadTotal NOTIFY fireDownloadProgressChanged)
	Q_PROPERTY(QString downloadFolder READ getDownloadFolder NOTIFY fireDownloadProgressChanged)

	Q_PROPERTY(QDateTime date READ getDate NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QString version READ getVersion NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl url READ getUrl NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(int size READ getSize NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl checksumUrl READ getChecksumUrl NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl notesUrl READ getNotesUrl() NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QString notes READ getNotes() NOTIFY fireAppUpdateDataChanged)

	private:
		bool mUpdateAvailable;
		bool mMissingPlatform;
		int mDownloadProgress;
		int mDownloadTotal;

		AppUpdateDataModel();
		~AppUpdateDataModel() override = default;

	private Q_SLOTS:
		void onAppcastFinished(bool pUpdateAvailable, const GlobalStatus& pStatus);
		void onAppDownloadProgress(qint64 pBytesReceived, qint64 pBytesTotal);
		void onAppDownloadFinished(const GlobalStatus& pError);

	public:
		[[nodiscard]] bool isUpdateAvailable() const;
		[[nodiscard]] bool isMissingPlatform() const;
		[[nodiscard]] bool isValid() const;
		[[nodiscard]] bool isCompatible() const;
		[[nodiscard]] int getDownloadProgress() const;
		[[nodiscard]] int getDownloadTotal() const;
		[[nodiscard]] QString getDownloadFolder() const;
		[[nodiscard]] const QDateTime& getDate() const;
		[[nodiscard]] const QString& getVersion() const;
		[[nodiscard]] const QUrl& getUrl() const;
		[[nodiscard]] int getSize() const;
		[[nodiscard]] const QUrl& getChecksumUrl() const;
		[[nodiscard]] const QUrl& getNotesUrl() const;
		[[nodiscard]] const QString& getNotes() const;
		[[nodiscard]] Q_INVOKABLE bool download();
		[[nodiscard]] Q_INVOKABLE bool abortDownload();

	Q_SIGNALS:
		void fireAppUpdateDataChanged();
		void fireDownloadProgressChanged();
		void fireAppUpdateFailed(GlobalStatus::Code pError);
		void fireAppDownloadFinished();
};

} // namespace governikus
