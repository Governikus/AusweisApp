/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
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

	Q_PROPERTY(bool valid READ isValid NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QDateTime date READ getDate NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QString version READ getVersion NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl url READ getUrl NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(int size READ getSize NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl checksumUrl READ getChecksumUrl NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl notesUrl READ getNotesUrl() NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QString notes READ getNotes() NOTIFY fireAppUpdateDataChanged)

	private:
		AppUpdateData mAppUpdateData;

	protected:
		AppUpdateDataModel();
		static AppUpdateDataModel& getInstance();

	public:
		bool isValid() const;
		const QDateTime getDate() const;
		const QString& getVersion() const;
		const QUrl& getUrl() const;
		int getSize() const;
		const QUrl& getChecksumUrl() const;
		const QUrl& getNotesUrl() const;
		const QString& getNotes() const;

	public Q_SLOTS:
		void onAppUpdateFinished(bool, const GlobalStatus&);

	Q_SIGNALS:
		void fireAppUpdateDataChanged();
};

} // namespace governikus
