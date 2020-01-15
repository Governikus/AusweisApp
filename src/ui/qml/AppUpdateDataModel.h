/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppUpdateData.h"
#include "Env.h"
#include "GlobalStatus.h"


namespace governikus
{
class AppUpdateDataModel
	: public QObject
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(bool updateAvailable READ isUpdateAvailable NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(bool valid READ isValid NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QDateTime date READ getDate NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QString version READ getVersion NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl url READ getUrl NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(int size READ getSize NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl checksumUrl READ getChecksumUrl NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl notesUrl READ getNotesUrl() NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QString notes READ getNotes() NOTIFY fireAppUpdateDataChanged)

	protected:
		AppUpdateDataModel();
		static AppUpdateDataModel& getInstance();

		bool mUpdateAvailable;

	private Q_SLOTS:
		void onAppUpdateCheckFinished(bool pUpdateAvailable, const GlobalStatus& pStatus);

	public:
		bool isUpdateAvailable() const;
		bool isValid() const;
		const QDateTime& getDate() const;
		const QString& getVersion() const;
		const QUrl& getUrl() const;
		int getSize() const;
		const QUrl& getChecksumUrl() const;
		const QUrl& getNotesUrl() const;
		const QString& getNotes() const;
		Q_INVOKABLE void skipUpdate() const;

	Q_SIGNALS:
		void fireAppUpdateDataChanged();
};

} // namespace governikus
