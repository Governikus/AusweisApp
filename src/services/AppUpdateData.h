/*!
 * \brief Update data implementation for application version.
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QJsonObject>
#include <QString>
#include <QUrl>
#include <QVersionNumber>

namespace governikus
{

class AppUpdateData
{
	private:
		QVersionNumber mMinOsVersion;
		QDateTime mDate;
		QString mVersion;
		QUrl mUrl;
		int mSize;
		QUrl mChecksumUrl;
		QUrl mNotesUrl;
		QString mNotes;
		QCryptographicHash::Algorithm mChecksumAlgorithm;
		QByteArray mChecksum;
		bool mChecksumValid;
		QString mUpdatePackagePath;
		GlobalStatus mParsingResult;

		static bool checkPlatformObject(const QJsonObject& pJson);
		static bool isPlatform(const QString& pPlatform);
		void verifyChecksum();

	public:
		explicit AppUpdateData(const GlobalStatus& pParsingResult = GlobalStatus::Code::No_Error);
		explicit AppUpdateData(const QByteArray& pData);

		bool isValid() const;
		const GlobalStatus& getParsingResult() const;

		bool isCompatible() const;
		const QDateTime& getDate() const;
		const QString& getVersion() const;
		const QUrl& getUrl() const;
		int getSize() const;
		const QUrl& getChecksumUrl() const;
		const QUrl& getNotesUrl() const;

		void setNotes(const QString& pNotes);
		const QString& getNotes() const;

		void setChecksum(const QByteArray& pChecksum, QCryptographicHash::Algorithm pAlgorithm);
		const QByteArray& getChecksum() const;
		bool isChecksumValid() const;

		void setUpdatePackagePath(const QString& pFile);
		QString getUpdatePackagePath() const;
};


} // namespace governikus
