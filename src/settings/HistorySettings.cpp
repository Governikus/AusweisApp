/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "HistorySettings.h"

#include <QDebug>
#include <QSettings>

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
}

using namespace governikus;


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
		const QString requestData = mStore->value(SETTINGS_NAME_CHRONIC_REQUESTED_DATA(), QString()).toString();
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
	if (appIsBackgroundService())
	{
		qDebug() << "Running as a background service. Ignoring save request for history.";
		return;
	}

	auto historyInfos = getHistoryInfos();
	historyInfos.prepend(pHistoryInfo);
	setHistoryInfos(historyInfos);
}


void HistorySettings::deleteSettings(const QDateTime& pLatestToKeep)
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
	setHistoryInfos(remainingItems);
}
