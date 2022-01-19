/*
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UserAgent.h"

#include "VersionNumber.h"

#include <QCoreApplication>

using namespace governikus;

UserAgent::UserAgent()
	: mMajor()
	, mMinor()
	, mSubminor()
{
	const auto& appVersion = VersionNumber::getApplicationVersion().getVersionNumber();
	mMajor = QString::number(appVersion.majorVersion());
	mMinor = QString::number(appVersion.minorVersion());
	mSubminor = QString::number(appVersion.microVersion());
}


QString UserAgent::getName() const
{
	return QCoreApplication::applicationName();
}


const QString& UserAgent::getVersionMajor() const
{
	return mMajor;
}


const QString& UserAgent::getVersionMinor() const
{
	return mMinor;
}


const QString& UserAgent::getVersionSubminor() const
{
	return mSubminor;
}
