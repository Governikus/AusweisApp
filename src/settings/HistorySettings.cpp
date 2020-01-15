/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "HistorySettings.h"

#include "AppSettings.h"
#include "Env.h"

#include <QLoggingCategory>

namespace
{
SETTINGS_NAME(SETTINGS_GROUP_NAME_CHRONIC, "history")
SETTINGS_NAME(SETTINGS_NAME_HISTORY_ITEMS, "items")
SETTINGS_NAME(SETTINGS_NAME_HISTORY_ENABLED, "enable")
SETTINGS_NAME(SETTINGS_NAME_CHRONIC_SUBJECTNAME, "subjectName")
SETTINGS_NAME(SETTINGS_NAME_CHRONIC_SUBJECTURL, "subjectUrl")
SETTINGS_NAME(SETTINGS_NAME_CHRONIC_USAGE, "usage")
SETTINGS_NAME(SETTINGS_NAME_CHRONIC_DATETIME, "dateTime")
SETTINGS_NAME(SETTINGS_NAME_CHRONIC_TOU, "termOfUsage")
SETTINGS_NAME(SETTINGS_NAME_CHRONIC_REQUESTED_DATA, "requestedData")
} // namespace

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(settings)

HistorySettings::HistorySettings()
	: AbstractSettings()
	, mStore(getStore())
{
	mStore->beginGroup(SETTINGS_GROUP_NAME_CHRONIC());
}


HistorySettings::~HistorySettings()
{
}


void HistorySettings::save()
{
	mStore->sync();
}


bool HistorySettings::isEnabled() const
{
	return mStore->value(SETTINGS_NAME_HISTORY_ENABLED(), true).toBool();
}


void HistorySettings::setEnabled(bool pEnabled)
{
	if (isEnabled() != pEnabled)
	{
		mStore->setValue(SETTINGS_NAME_HISTORY_ENABLED(), pEnabled);
		Q_EMIT fireEnabledChanged(pEnabled);
	}
}


QVector<HistoryInfo> HistorySettings::getHistoryInfos() const
{
	const int itemCount = mStore->beginReadArray(SETTINGS_NAME_HISTORY_ITEMS());

	QVector<HistoryInfo> historyInfos;
	historyInfos.reserve(itemCount);
	for (int i = 0; i < itemCount; ++i)
	{
		mStore->setArrayIndex(i);
		const QString subjectName = mStore->value(SETTINGS_NAME_CHRONIC_SUBJECTNAME(), QString()).toString();
		const QString subjectUrl = mStore->value(SETTINGS_NAME_CHRONIC_SUBJECTURL(), QString()).toString();
		const QString usage = mStore->value(SETTINGS_NAME_CHRONIC_USAGE(), QString()).toString();
		const QDateTime dateTime = QDateTime::fromString(mStore->value(SETTINGS_NAME_CHRONIC_DATETIME(), QString()).toString(), Qt::ISODate);
		const QString termsOfUsage = mStore->value(SETTINGS_NAME_CHRONIC_TOU(), QString()).toString();
		const QStringList requestData = mStore->value(SETTINGS_NAME_CHRONIC_REQUESTED_DATA(), QStringList()).toStringList();
		historyInfos += HistoryInfo(subjectName, subjectUrl, usage, dateTime, termsOfUsage, requestData);
	}

	mStore->endArray();
	return historyInfos;
}


void HistorySettings::setHistoryInfos(const QVector<HistoryInfo>& pHistoryInfos)
{
	mStore->beginGroup(SETTINGS_NAME_HISTORY_ITEMS());
	mStore->remove(QString());
	mStore->endGroup();

	mStore->beginWriteArray(SETTINGS_NAME_HISTORY_ITEMS());
	for (int i = 0; i < pHistoryInfos.size(); ++i)
	{
		const HistoryInfo& item = pHistoryInfos.at(i);

		mStore->setArrayIndex(i);
		mStore->setValue(SETTINGS_NAME_CHRONIC_SUBJECTNAME(), item.getSubjectName());
		mStore->setValue(SETTINGS_NAME_CHRONIC_SUBJECTURL(), item.getSubjectUrl());
		mStore->setValue(SETTINGS_NAME_CHRONIC_USAGE(), item.getPurpose());
		mStore->setValue(SETTINGS_NAME_CHRONIC_DATETIME(), item.getDateTime().toString(Qt::ISODate));
		mStore->setValue(SETTINGS_NAME_CHRONIC_TOU(), item.getTermOfUsage());
		mStore->setValue(SETTINGS_NAME_CHRONIC_REQUESTED_DATA(), item.getRequestedData());
	}
	mStore->endArray();

	Q_EMIT fireHistoryInfosChanged();
}


void HistorySettings::addHistoryInfo(const HistoryInfo& pHistoryInfo)
{
	if (Env::getSingleton<AppSettings>()->isUsedAsSDK())
	{
		qCDebug(settings) << "Running as SDK. Ignoring save request for history.";
		return;
	}

	auto historyInfos = getHistoryInfos();
	historyInfos.prepend(pHistoryInfo);
	setHistoryInfos(historyInfos);
}


int HistorySettings::deleteSettings(const QDateTime& pLatestToKeep)
{
	const auto historyInfos = getHistoryInfos();
	QVector<HistoryInfo> remainingItems;
	for (const auto& item : historyInfos)
	{
		if (!pLatestToKeep.isNull() && item.getDateTime() <= pLatestToKeep)
		{
			remainingItems += item;
		}
	}
	int numberOfItemsToRemove = historyInfos.size() - remainingItems.size();
	setHistoryInfos(remainingItems);
	return numberOfItemsToRemove;
}


int HistorySettings::deleteSettings(const TimePeriod& pPeriodToRemove)
{
	QDateTime latestToKeep = QDateTime::currentDateTime();
	switch (pPeriodToRemove)
	{
		case TimePeriod::PAST_HOUR:
			latestToKeep = latestToKeep.addSecs(-60 * 60);
			break;

		case TimePeriod::PAST_DAY:
			latestToKeep = latestToKeep.addDays(-1);
			break;

		case TimePeriod::PAST_WEEK:
			latestToKeep = latestToKeep.addDays(-7);
			break;

		case TimePeriod::LAST_FOUR_WEEKS:
			latestToKeep = latestToKeep.addDays(-7 * 4);
			break;

		case TimePeriod::ALL_HISTORY:
			latestToKeep = QDateTime::fromMSecsSinceEpoch(1);
			break;

		case TimePeriod::UNKNOWN:
			return 0;
	}

	qCDebug(settings) << "Remove history entries until timestamp:" << latestToKeep;
	return deleteSettings(latestToKeep);
}
