/*!
 * \brief Update data implementation for application version.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QDateTime>
#include <QJsonObject>
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
		GlobalStatus mParsingResult;

		static bool checkPlatformObject(const QJsonObject& pJson);
		static bool isPlatform(const QString& pPlatform);

	public:
		AppUpdateData();
		AppUpdateData(GlobalStatus pParsingResult);

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
		void setChecksumUrl(const QUrl& pChecksumUrl);

		const QUrl& getNotesUrl() const;
		void setNotesUrl(const QUrl& pNotesUrl);

		const QString& getNotes() const;
		void setNotes(const QString& pNotes);

		const GlobalStatus& getParsingResult();

		static AppUpdateData parse(const QByteArray& pData);
};


} // namespace governikus
