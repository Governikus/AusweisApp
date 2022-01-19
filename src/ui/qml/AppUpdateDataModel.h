/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppUpdateData.h"
#include "Env.h"
#include "GlobalStatus.h"

#include <QUrl>


namespace governikus
{
class AppUpdateDataModel
	: public QObject
{
	Q_OBJECT
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
		bool isUpdateAvailable() const;
		bool isMissingPlatform() const;
		bool isValid() const;
		bool isCompatible() const;
		int getDownloadProgress() const;
		int getDownloadTotal() const;
		QString getDownloadFolder() const;
		const QDateTime& getDate() const;
		const QString& getVersion() const;
		const QUrl& getUrl() const;
		int getSize() const;
		const QUrl& getChecksumUrl() const;
		const QUrl& getNotesUrl() const;
		const QString& getNotes() const;
		Q_INVOKABLE void skipUpdate() const;
		Q_INVOKABLE bool download();
		Q_INVOKABLE bool abortDownload();

	Q_SIGNALS:
		void fireAppUpdateDataChanged();
		void fireDownloadProgressChanged();
		void fireAppUpdateFailed(GlobalStatus::Code pError);
		void fireAppDownloadFinished();
};

} // namespace governikus
