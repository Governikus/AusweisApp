/*!
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#include "HistoryInfo.h"

#include <QDebug>
#include <QSettings>


using namespace governikus;


HistoryInfo::HistoryInfo(const QString& pSubjectName, const QString& pSubjectUrl, const QString& pUsage, const QDateTime& pDateTime, const QString& pTermOfUsage, const QStringList& pRequestedData)
	: mSubjectName(pSubjectName)
	, mSubjectUrl(pSubjectUrl)
	, mPurpose(pUsage)
	, mDateTime(roundToSeconds(pDateTime))
	, mTermOfUsage(pTermOfUsage)
	, mRequestedData(pRequestedData)
{
}


QDateTime HistoryInfo::roundToSeconds(const QDateTime& pDateTime)
{
	QDateTime roundedDateTime;
	roundedDateTime.setMSecsSinceEpoch(pDateTime.toMSecsSinceEpoch() - pDateTime.toMSecsSinceEpoch() % 1000);
	return roundedDateTime;
}


const QStringList& HistoryInfo::getRequestedData() const
{
	return mRequestedData;
}


const QString& HistoryInfo::getTermOfUsage() const
{
	return mTermOfUsage;
}


const QDateTime& HistoryInfo::getDateTime() const
{
	return mDateTime;
}


const QString& HistoryInfo::getPurpose() const
{
	return mPurpose;
}


const QString& HistoryInfo::getSubjectName() const
{
	return mSubjectName;
}


const QString& HistoryInfo::getSubjectUrl() const
{
	return mSubjectUrl;
}
