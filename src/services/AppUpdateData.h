/*!
 * \brief Update data implementation for application version.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
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

		[[nodiscard]] bool isValid() const;
		[[nodiscard]] const GlobalStatus& getParsingResult() const;

		[[nodiscard]] bool isCompatible() const;
		[[nodiscard]] const QDateTime& getDate() const;
		[[nodiscard]] const QString& getVersion() const;
		[[nodiscard]] const QUrl& getUrl() const;
		[[nodiscard]] int getSize() const;
		[[nodiscard]] const QUrl& getChecksumUrl() const;
		[[nodiscard]] const QUrl& getNotesUrl() const;

		void setNotes(const QString& pNotes);
		[[nodiscard]] const QString& getNotes() const;

		void setChecksum(const QByteArray& pChecksum, QCryptographicHash::Algorithm pAlgorithm);
		[[nodiscard]] const QByteArray& getChecksum() const;
		[[nodiscard]] bool isChecksumValid() const;

		void setUpdatePackagePath(const QString& pFile);
		[[nodiscard]] QString getUpdatePackagePath() const;
};


} // namespace governikus
