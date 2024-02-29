/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "VersionNumber.h"

#include <QCoreApplication>
#include <QGlobalStatic>
#include <QStringView>

using namespace governikus;

Q_GLOBAL_STATIC_WITH_ARGS(VersionNumber, AppVersionNumber, (QCoreApplication::applicationVersion()))


VersionNumber::VersionNumber(const QString& pVersion)
	: mVersionNumber()
	, mSuffix()
{
	qsizetype idx = 0;
	mVersionNumber = QVersionNumber::fromString(pVersion, &idx);
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
	return mVersionNumber.isNull() || isBetaVersion() || !mSuffix.isEmpty();
}


bool VersionNumber::isBetaVersion() const
{
	return mVersionNumber.minorVersion() >= 100 || mVersionNumber.microVersion() >= 100;
}


auto VersionNumber::getInfoFromSuffix(QLatin1Char pStart, QLatin1Char pEnd) const
{
	QStringView view;

	if (const auto indexStart = mSuffix.indexOf(pStart) + 1; indexStart > 0)
	{
		view = QStringView(mSuffix).sliced(indexStart);
		if (const auto indexEnd = view.indexOf(pEnd); indexEnd > 0)
		{
			view = view.sliced(0, indexEnd);
		}
	}

	return view;
}


int VersionNumber::getDistance() const
{
	if (const auto view = getInfoFromSuffix(QLatin1Char('+')); !view.isEmpty())
	{
		bool ok;
		int value = view.toInt(&ok);
		if (ok)
		{
			return value;
		}
	}

	return -1;
}


QString VersionNumber::getBranch() const
{
	return getInfoFromSuffix(QLatin1Char('-')).toString();
}


QString VersionNumber::getRevision() const
{
	if (mSuffix.count(QLatin1Char('-')) > 1)
	{
		if (const auto index = mSuffix.lastIndexOf(QLatin1Char('-')) + 1; index > 0)
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


QString VersionNumber::getSuffix() const
{
	return mSuffix;
}
