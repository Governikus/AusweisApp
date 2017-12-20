/*
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "VersionNumber.h"

#include <QCoreApplication>
#include <QGlobalStatic>

using namespace governikus;

Q_GLOBAL_STATIC_WITH_ARGS(VersionNumber, AppVersionNumber, (QCoreApplication::applicationVersion()))


VersionNumber::VersionNumber(const QString& pVersion)
	: mVersionNumber()
	, mSuffix()
{
	int idx = 0;
	mVersionNumber = QVersionNumber::fromString(pVersion, &idx);
#ifdef Q_CC_GNU
	__sync_synchronize(); // a gcc bug: https://bugs.alpinelinux.org/issues/7584
#endif
	mSuffix = pVersion.mid(idx).trimmed();
}


const VersionNumber& VersionNumber::getApplicationVersion()
{
	return *AppVersionNumber;
}


const QVersionNumber& VersionNumber::getVersionNumber() const
{
	return mVersionNumber;
}


bool VersionNumber::isDeveloperVersion() const
{
	return mVersionNumber.isNull() || (mVersionNumber.minorVersion() & 1) || !mSuffix.isEmpty();
}


int VersionNumber::getDistance() const
{
	const int indexStart = mSuffix.indexOf(QLatin1Char('+')) + 1;
	const int indexEnd = mSuffix.indexOf(QLatin1Char('-'), indexStart);
	if (indexStart && indexEnd)
	{
		bool ok;
		int value = mSuffix.mid(indexStart, indexEnd - indexStart).toInt(&ok);
		if (ok)
		{
			return value;
		}
	}

	return -1;
}


QString VersionNumber::getBranch() const
{
	const int indexStart = mSuffix.indexOf(QLatin1Char('-')) + 1;
	const int indexEnd = mSuffix.indexOf(QLatin1Char('-'), indexStart);
	if (indexStart && indexEnd)
	{
		return mSuffix.mid(indexStart, indexEnd - indexStart);
	}

	return QString();
}


QString VersionNumber::getRevision() const
{
	if (mSuffix.count(QLatin1Char('-')) > 1)
	{
		const int index = mSuffix.lastIndexOf(QLatin1Char('-')) + 1;
		if (index)
		{
			return mSuffix.mid(index);
		}
	}

	return QString();
}


bool VersionNumber::isDraft() const
{
	return mSuffix.contains(QStringLiteral("-draft")) || mSuffix.contains(QStringLiteral("-secret"));
}
