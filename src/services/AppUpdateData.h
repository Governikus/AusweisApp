/*!
 * \brief Update data implementation for application version.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QDateTime>
#include <QString>
#include <QUrl>

namespace governikus
{

class AppUpdateData
{
	private:
		QDateTime mDate;
		QString mVersion;
		QUrl mUrl;
		int mSize;
		QUrl mChecksumUrl;
		QUrl mNotesUrl;
		QString mNotes;

	public:
		AppUpdateData();

		bool isValid() const;

		const QDateTime getDate() const;
		void setDate(const QDateTime& pDate);

		const QString& getVersion() const;
		void setVersion(const QString& pVersion);

		const QUrl& getUrl() const;
		void setUrl(const QUrl& pUrl);

		int getSize() const;
		void setSize(int pSize);

		const QUrl& getChecksumUrl() const;
		void setChecksumUrl(const QUrl& pUrl);

		const QUrl& getNotesUrl() const;
		void setNotesUrl(const QUrl& pUrl);

		const QString& getNotes() const;
		void setNotes(const QString& pNotes);
};


} /* namespace governikus */
