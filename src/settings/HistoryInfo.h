/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Represents history settings.
 */

#pragma once

#include "AbstractSettings.h"

#include <QDateTime>
#include <QString>


namespace governikus
{

/*!
 * \brief Represents a history entry, i.e. information of a successful authentication
 */
class HistoryInfo
{
	private:
		static QDateTime roundToSeconds(const QDateTime& pDateTime);

		/*!
		 * The subject name contained in the certificate description
		 */
		QString mSubjectName;

		/*!
		 * The subject url contained in the certificate description
		 */
		QString mSubjectUrl;

		/*!
		 * The purpose contained in the certificate description
		 */
		QString mPurpose;

		/*!
		 * The authentication date and time
		 */
		QDateTime mDateTime;

		/*!
		 * The terms of usage contained in the certificate description
		 */
		QString mTermOfUsage;

		/*!
		 * The requested data fields during authentication
		 */
		QStringList mRequestedData;

	public:
		HistoryInfo() = default;

		HistoryInfo(const QString& pSubjectName, const QString& pSubjectUrl, const QString& pUsage, const QDateTime& pDateTime, const QString& pTermOfUsage, const QStringList& pRequestedData);

		bool operator==(const HistoryInfo& pOther) const
		{
			return mSubjectName == pOther.mSubjectName && mSubjectUrl == pOther.mSubjectUrl && mPurpose == pOther.mPurpose
				   && mDateTime == pOther.mDateTime && mTermOfUsage == pOther.mTermOfUsage && mRequestedData == pOther.mRequestedData;
		}


		bool operator!=(const HistoryInfo& pOther) const
		{
			return !(*this == pOther);
		}


		[[nodiscard]] const QString& getSubjectName() const;
		[[nodiscard]] const QString& getSubjectUrl() const;
		[[nodiscard]] const QString& getPurpose() const;
		[[nodiscard]] const QDateTime& getDateTime() const;
		[[nodiscard]] const QString& getTermOfUsage() const;
		[[nodiscard]] const QStringList& getRequestedData() const;
};


} // namespace governikus
