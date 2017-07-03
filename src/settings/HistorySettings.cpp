/*!
 * HistorySetting.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "HistorySettings.h"

#include <QSettings>


// names for settings groups/keys
const QLatin1String SETTINGS_GROUP_NAME_CHRONIC("history");
const QLatin1String SETTINGS_NAME_HISTORY_ITEMS("items");
const QLatin1String SETTINGS_NAME_HISTORY_ENABLED("enable");
const QLatin1String SETTINGS_NAME_CHRONIC_SUBJECTNAME("subjectName");
const QLatin1String SETTINGS_NAME_CHRONIC_SUBJECTURL("subjectUrl");
const QLatin1String SETTINGS_NAME_CHRONIC_USAGE("usage");
const QLatin1String SETTINGS_NAME_CHRONIC_DATETIME("dateTime");
const QLatin1String SETTINGS_NAME_CHRONIC_TOU("termOfUsage");
const QLatin1String SETTINGS_NAME_CHRONIC_REQUESTED_DATA("requestedData");


using namespace governikus;


HistoryEntry::HistoryEntry(const QString& pSubjectName, const QString& pSubjectUrl, const QString& pUsage, const QDateTime& pDateTime, const QString& pTermOfUsage, const QString& pRequestedData)
	: mSubjectName(pSubjectName)
	, mSubjectUrl(pSubjectUrl)
	, mPurpose(pUsage)
	, mDateTime(roundToSeconds(pDateTime))
	, mTermOfUsage(pTermOfUsage)
	, mRequestedData(pRequestedData)
{
}


QDateTime HistoryEntry::roundToSeconds(const QDateTime& pDateTime)
{
	QDateTime roundedDateTime;
	roundedDateTime.setMSecsSinceEpoch(pDateTime.toMSecsSinceEpoch() - pDateTime.toMSecsSinceEpoch() % 1000);
	return roundedDateTime;
}


const QString& HistoryEntry::getRequestedData() const
{
	return mRequestedData;
}


const QString& HistoryEntry::getTermOfUsage() const
{
	return mTermOfUsage;
}


const QDateTime& HistoryEntry::getDateTime() const
{
	return mDateTime;
}


const QString& HistoryEntry::getPurpose() const
{
	return mPurpose;
}


const QString& HistoryEntry::getSubjectName() const
{
	return mSubjectName;
}


const QString& HistoryEntry::getSubjectUrl() const
{
	return mSubjectUrl;
}


HistorySettings::HistorySettings()
	: AbstractSettings()
	, mEnabled(true)
	, mHistoryEntries()
{
	connect(this, &HistorySettings::fireEnabledChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(this, &HistorySettings::fireHistoryEntriesChanged, this, &AbstractSettings::fireSettingsChanged);
}


HistorySettings::~HistorySettings()
{
}


void HistorySettings::load()
{
	auto settings = AbstractSettings::getStore();
	settings->beginGroup(SETTINGS_GROUP_NAME_CHRONIC);

	setEnabled(settings->value(SETTINGS_NAME_HISTORY_ENABLED, true).toBool());

	const int itemCount = settings->beginReadArray(SETTINGS_NAME_HISTORY_ITEMS);
	QVector<HistoryEntry> items;
	items.reserve(itemCount);
	for (int i = 0; i < itemCount; ++i)
	{
		settings->setArrayIndex(i);
		QString subjectName = settings->value(SETTINGS_NAME_CHRONIC_SUBJECTNAME, "").toString();
		QString subjectUrl = settings->value(SETTINGS_NAME_CHRONIC_SUBJECTURL, "").toString();
		QString usage = settings->value(SETTINGS_NAME_CHRONIC_USAGE, "").toString();
		QDateTime dateTime = QDateTime::fromString(settings->value(SETTINGS_NAME_CHRONIC_DATETIME, "").toString(), Qt::ISODate);
		QString termsOfUsage = settings->value(SETTINGS_NAME_CHRONIC_TOU, "").toString();
		QString requestData = settings->value(SETTINGS_NAME_CHRONIC_REQUESTED_DATA, "").toString();
		items.append(HistoryEntry(subjectName, subjectUrl, usage, dateTime, termsOfUsage, requestData));
	}
	settings->endArray();
	setHistoryEntries(items);

	settings->endGroup();
}


bool HistorySettings::isUnsaved() const
{
	HistorySettings oldSettings;
	oldSettings.load();
	return oldSettings != *this;
}


void HistorySettings::save()
{
	auto settings = AbstractSettings::getStore();
	settings->beginGroup(SETTINGS_GROUP_NAME_CHRONIC);
	settings->remove(QString()); // remove the whole group first

	settings->setValue(SETTINGS_NAME_HISTORY_ENABLED, mEnabled);

	settings->beginWriteArray(SETTINGS_NAME_HISTORY_ITEMS);
	const int itemCount = mHistoryEntries.count();
	for (int i = 0; i < itemCount; ++i)
	{
		const HistoryEntry& item = mHistoryEntries.at(i);
		settings->setArrayIndex(i);
		settings->setValue(SETTINGS_NAME_CHRONIC_SUBJECTNAME, item.getSubjectName());
		settings->setValue(SETTINGS_NAME_CHRONIC_SUBJECTURL, item.getSubjectUrl());
		settings->setValue(SETTINGS_NAME_CHRONIC_USAGE, item.getPurpose());
		settings->setValue(SETTINGS_NAME_CHRONIC_DATETIME, item.getDateTime().toString(Qt::ISODate));
		settings->setValue(SETTINGS_NAME_CHRONIC_TOU, item.getTermOfUsage());
		settings->setValue(SETTINGS_NAME_CHRONIC_REQUESTED_DATA, item.getRequestedData());
	}
	settings->endArray();

	settings->endGroup();
	settings->sync();
}


void HistorySettings::deleteSettings(const QDateTime& pLatestToKeep)
{
	QVector<HistoryEntry> remainingItems;
	for (const auto& item : qAsConst(mHistoryEntries))
	{
		if (!pLatestToKeep.isNull() && item.getDateTime() <= pLatestToKeep)
		{
			remainingItems.append(item);
		}
	}
	setHistoryEntries(remainingItems);
}


bool HistorySettings::isEnabled() const
{
	return mEnabled;
}


void HistorySettings::setEnabled(bool pEnabled)
{
	if (mEnabled != pEnabled)
	{
		mEnabled = pEnabled;
		Q_EMIT fireEnabledChanged();
	}
}


const QVector<HistoryEntry>& HistorySettings::getHistoryEntries() const
{
	return mHistoryEntries;
}


void HistorySettings::setHistoryEntries(const QVector<HistoryEntry>& pHistoryEntries)
{
	if (mHistoryEntries != pHistoryEntries)
	{
		mHistoryEntries = pHistoryEntries;
		Q_EMIT fireHistoryEntriesChanged();
	}
}


void HistorySettings::addHistoryEntry(const HistoryEntry& pHistoryEntry)
{
	mHistoryEntries.prepend(pHistoryEntry);
	Q_EMIT fireHistoryEntriesChanged();
}
