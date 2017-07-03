/*!
 * HistorySetting.h
 *
 * \brief Represents history settings.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractSettings.h"

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVector>

class test_HistorySettings;

namespace governikus
{

/*!
 * \brief Represents a history entry, i.e. information of a successfull authentication
 */
class HistoryEntry
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
		QString mRequestedData;

	public:
		HistoryEntry()
		{
		}


		HistoryEntry(const QString& pSubjectName, const QString& pSubjectUrl, const QString& pUsage, const QDateTime& pDateTime, const QString& pTermOfUsage, const QString& pRequestedData);

		bool operator==(const HistoryEntry& pOther) const
		{
			return mSubjectName == pOther.mSubjectName && mSubjectUrl == pOther.mSubjectUrl && mPurpose == pOther.mPurpose
				   && mDateTime == pOther.mDateTime && mTermOfUsage == pOther.mTermOfUsage && mRequestedData == pOther.mRequestedData;
		}


		bool operator!=(const HistoryEntry& pOther) const
		{
			return !(*this == pOther);
		}


		const QString& getSubjectName() const;
		const QString& getSubjectUrl() const;
		const QString& getPurpose() const;
		const QDateTime& getDateTime() const;
		const QString& getTermOfUsage() const;
		const QString& getRequestedData() const;
};

class HistorySettings
	: public AbstractSettings
{
	Q_OBJECT
	friend class AppSettings;
	friend class::test_HistorySettings;

	private:
		bool mEnabled;
		QVector<HistoryEntry> mHistoryEntries;

		HistorySettings();

	public:
		virtual ~HistorySettings();

		virtual void load() override;

		virtual bool isUnsaved() const override;

		virtual void save() override;

		void deleteSettings(const QDateTime& pLatestToKeep = QDateTime());

		bool isEnabled() const;
		void setEnabled(bool pEnabled);

		const QVector<HistoryEntry>& getHistoryEntries() const;
		void setHistoryEntries(const QVector<HistoryEntry>& pHistoryEntries);
		void addHistoryEntry(const HistoryEntry& pHistoryEntry);

	Q_SIGNALS:
		void fireEnabledChanged();
		void fireHistoryEntriesChanged();
};

inline bool operator==(const HistorySettings& pLeft, const HistorySettings& pRight)
{
	return &pLeft == &pRight || (
		pLeft.isEnabled() == pRight.isEnabled() &&
		pLeft.getHistoryEntries() == pRight.getHistoryEntries());
}


inline bool operator!=(const HistorySettings& pLeft, const HistorySettings& pRight)
{
	return !(pLeft == pRight);
}


} /* namespace governikus */
