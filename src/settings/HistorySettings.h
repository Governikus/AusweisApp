/*!
 * \brief Represents history settings.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractSettings.h"

#include "EnumHelper.h"
#include "HistoryInfo.h"

#include <QVector>


class test_HistorySettings;


namespace governikus
{

defineEnumType(TimePeriod,
		PAST_HOUR,
		PAST_DAY,
		PAST_WEEK,
		LAST_FOUR_WEEKS,
		ALL_HISTORY,
		UNKNOWN
		)

class HistorySettings
	: public AbstractSettings
{
	Q_OBJECT
	friend class AppSettings;

	private:
		QSharedPointer<QSettings> mStore;
		QVector<HistoryInfo> mHistoryInfos;

		HistorySettings();
		QVector<HistoryInfo> getHistoryInfosFromStore() const;

	public:
		virtual ~HistorySettings() override;
		virtual void save() override;

		bool isEnabled() const;
		void setEnabled(bool pEnabled);

		const QVector<HistoryInfo>& getHistoryInfos() const;
		void setHistoryInfos(const QVector<HistoryInfo>& pHistoryInfos);
		void addHistoryInfo(const HistoryInfo& pHistoryInfo);
		int deleteSettings(const QDateTime& pLatestToKeep = QDateTime());
		int deleteSettings(const TimePeriod& pPeriodToRemove);

	Q_SIGNALS:
		void fireEnabledChanged(bool pValue);
		void fireHistoryInfosChanged();
};


} // namespace governikus
