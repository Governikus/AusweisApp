/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
	qsizetype idx = 0;
#else
	int idx = 0;
#endif

	mVersionNumber = QVersionNumber::fromString(pVersion, &idx);
#ifdef Q_CC_GNU
	__sync_synchronize(); // QTBUG-62185
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
	return mVersionNumber.isNull() || isBetaVersion() || !mSuffix.isEmpty();
}


bool VersionNumber::isBetaVersion() const
{
	return mVersionNumber.minorVersion() & 1;
}


auto VersionNumber::getInfoFromSuffix(QLatin1Char pStart, QLatin1Char pEnd) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	QStringView view;
#else
	QStringRef view;
#endif

	if (const auto indexStart = mSuffix.indexOf(pStart) + 1; indexStart > 0)
	{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
		view = QStringView(mSuffix).sliced(indexStart);
#else
		view = mSuffix.midRef(indexStart);
#endif

		if (const auto indexEnd = view.indexOf(pEnd); indexEnd > 0)
		{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			view = view.sliced(0, indexEnd);
#else
			view = view.mid(0, indexEnd);
#endif
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
