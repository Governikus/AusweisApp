/*!
 * \brief Represents history settings.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractSettings.h"

#include "HistoryInfo.h"

#include <QVector>


class test_HistorySettings;


namespace governikus
{

class HistorySettings
	: public AbstractSettings
{
	Q_OBJECT
	friend class AppSettings;
	friend class ::test_HistorySettings;

	private:
		QSharedPointer<QSettings> mStore;

		HistorySettings();

	public:
		virtual ~HistorySettings() override;
		virtual void save() override;

		bool isEnabled() const;
		void setEnabled(bool pEnabled);

		QVector<HistoryInfo> getHistoryInfos() const;
		void setHistoryInfos(const QVector<HistoryInfo>& pHistoryInfos);
		void addHistoryInfo(const HistoryInfo& pHistoryInfo);
		void deleteSettings(const QDateTime& pLatestToKeep = QDateTime());

	Q_SIGNALS:
		void fireEnabledChanged(bool pValue);
		void fireHistoryInfosChanged();
};


} /* namespace governikus */
