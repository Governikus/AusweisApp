/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */


#include "IfdVersion.h"


using namespace governikus;


IfdVersion::IfdVersion(IfdVersion::Version pVersion)
	: mVersion(pVersion)
{
}


IfdVersion::IfdVersion(const QString& pVersionString)
	: IfdVersion(fromString(pVersionString))
{
}


bool IfdVersion::isValid() const
{
	return mVersion != Version::Unknown;
}


IfdVersion::Version IfdVersion::getVersion() const
{
	return mVersion;
}


QString IfdVersion::toString() const
{
	switch (mVersion)
	{
		case IfdVersion::Version::Unknown:
			return QString();

		case IfdVersion::Version::v0:
			return QStringLiteral("IFDInterface_WebSocket_v0");

#ifndef QT_NO_DEBUG
		case IfdVersion::Version::v_test:
			return QStringLiteral("IFDInterface_WebSocket_v_test");

#endif
	}

	Q_UNREACHABLE();
}


IfdVersion::Version IfdVersion::fromString(const QString& pVersionString)
{
	const IfdVersion& v0 = Version::v0;
	if (pVersionString == v0.toString())
	{
		return Version::v0;
	}

#ifndef QT_NO_DEBUG
	if (pVersionString == IfdVersion(Version::v_test).toString())
	{
		return Version::v_test;
	}
#endif

	return Version::Unknown;
}


QVector<IfdVersion::Version> IfdVersion::supported()
{
	return QVector<IfdVersion::Version>({Version::v0});
}


IfdVersion::Version IfdVersion::selectLatestSupported(const QVector<IfdVersion::Version>& pVersions)
{
	QVector<IfdVersion::Version> supported;
	supported += IfdVersion::Version::Unknown;

	for (const IfdVersion version : pVersions)
	{
		if (version.isSupported())
		{
			supported += version.getVersion();
		}
	}

	Q_ASSERT(!supported.isEmpty());
	return supported.last();
}


bool IfdVersion::isSupported() const
{
	const QVector<Version>& supportedVersions = supported();
	for (const auto& supportedVersion : supportedVersions)
	{
		if (mVersion == supportedVersion)
		{
			return true;
		}
	}

	return false;
}


bool IfdVersion::operator==(const IfdVersion& pOther) const
{
	return mVersion == pOther.mVersion;
}


bool IfdVersion::operator!=(const IfdVersion& pOther) const
{
	return !(*this == pOther);
}
