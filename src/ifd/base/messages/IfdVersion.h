/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDebug>
#include <QString>
#include <QVector>

namespace governikus
{
class IfdVersion
{
	public:
		enum class Version : int
		{
			Unknown = -1,
			v0,
			v2,
			latest = v2
		};

	private:
		const Version mVersion;

		static Version fromString(const QString& pVersionString);

	public:
		explicit IfdVersion(Version pVersion = Version::Unknown);
		explicit IfdVersion(const QString& pVersionString);

		[[nodiscard]] Version getVersion() const;
		[[nodiscard]] bool isValid() const;
		[[nodiscard]] bool isSupported() const;

		bool operator==(const IfdVersion& pOther) const;
		bool operator!=(const IfdVersion& pOther) const;

		[[nodiscard]] QString toString() const;

		static QVector<Version> supported();
		static Version selectLatestSupported(const QVector<Version>& pVersions);
};


inline QDebug operator<<(QDebug pDbg, const IfdVersion& pVersion)
{
	pDbg << pVersion.toString();
	return pDbg;
}


inline QDebug operator<<(QDebug pDbg, const IfdVersion::Version& pVersion)
{
	pDbg << IfdVersion(pVersion).toString();
	return pDbg;
}


#ifndef QT_NO_DEBUG
inline bool operator ==(const IfdVersion& pLeft, const IfdVersion::Version& pRight)
{
	return pLeft.getVersion() == pRight;
}


#endif
} // namespace governikus
