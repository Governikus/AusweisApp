/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
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


auto VersionNumber::getInfoFromSuffix(QLatin1Char pStart, QLatin1Char pEnd) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	QStringView view;
#else
	QStringRef view;
#endif

	const auto indexStart = mSuffix.indexOf(pStart) + 1;
	if (indexStart > 0)
	{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
		view = QStringView(mSuffix).sliced(indexStart);
#else
		view = mSuffix.midRef(indexStart);
#endif

		const auto indexEnd = view.indexOf(pEnd);
		if (indexEnd > 0)
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
	const auto view = getInfoFromSuffix(QLatin1Char('+'));

	if (!view.isEmpty())
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
		const auto index = mSuffix.lastIndexOf(QLatin1Char('-')) + 1;
		if (index > 0)
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
