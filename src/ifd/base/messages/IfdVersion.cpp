/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */


#include "IfdVersion.h"


using namespace governikus;


IfdVersion::Version IfdVersion::fromString(const QString& pVersionString)
{
	if (pVersionString == IfdVersion(Version::v0).toString())
	{
		return Version::v0;
	}

	if (pVersionString == IfdVersion(Version::v2).toString())
	{
		return Version::v2;
	}

	return Version::Unknown;
}


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

		case IfdVersion::Version::v2:
			return QStringLiteral("IFDInterface_WebSocket_v2");
	}

	Q_UNREACHABLE();
}


QVector<IfdVersion::Version> IfdVersion::supported()
{
	return QVector<IfdVersion::Version>({Version::v2});
}


IfdVersion::Version IfdVersion::selectLatestSupported(const QVector<IfdVersion::Version>& pVersions)
{
	QVector<IfdVersion::Version> sorted(pVersions);
	std::sort(sorted.rbegin(), sorted.rend());

	for (const IfdVersion::Version version : sorted)
	{
		if (IfdVersion ifdVersion(version); ifdVersion.isSupported())
		{
			return ifdVersion.getVersion();
		}
	}

	return Version::Unknown;
}


bool IfdVersion::isSupported() const
{
	return supported().contains(mVersion);
}


bool IfdVersion::operator==(const IfdVersion& pOther) const
{
	return mVersion == pOther.mVersion;
}


bool IfdVersion::operator!=(const IfdVersion& pOther) const
{
	return !(*this == pOther);
}
