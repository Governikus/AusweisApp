/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "AppUpdateData.h"

using namespace governikus;

AppUpdateData::AppUpdateData()
	: mDate()
	, mVersion()
	, mUrl()
	, mSize(-1)
	, mChecksumUrl()
	, mNotesUrl()
	, mNotes()
{
}


bool AppUpdateData::isValid() const
{
	// Valid means = required data!
	return !mVersion.isEmpty() &&
		   mUrl.isValid() &&
		   mNotesUrl.isValid();
}


const QDateTime AppUpdateData::getDate() const
{
	return mDate;
}


void AppUpdateData::setDate(const QDateTime& pDate)
{
	mDate = pDate;
}


const QString& AppUpdateData::getVersion() const
{
	return mVersion;
}


void AppUpdateData::setVersion(const QString& pVersion)
{
	mVersion = pVersion;
}


const QUrl& AppUpdateData::getUrl() const
{
	return mUrl;
}


void AppUpdateData::setUrl(const QUrl& pUrl)
{
	mUrl = pUrl;
}


int AppUpdateData::getSize() const
{
	return mSize;
}


void AppUpdateData::setSize(int pSize)
{
	if (pSize < 0)
	{
		mSize = -1;
	}
	else
	{
		mSize = pSize;
	}
}


const QUrl& AppUpdateData::getChecksumUrl() const
{
	return mChecksumUrl;
}


void AppUpdateData::setChecksumUrl(const QUrl& pUrl)
{
	mChecksumUrl = pUrl;
}


const QUrl& AppUpdateData::getNotesUrl() const
{
	return mNotesUrl;
}


void AppUpdateData::setNotesUrl(const QUrl& pUrl)
{
	mNotesUrl = pUrl;
}


void AppUpdateData::setNotes(const QString& pNotes)
{
	mNotes = pNotes;
}


const QString& AppUpdateData::getNotes() const
{
	return mNotes;
}
