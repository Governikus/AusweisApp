/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
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
		IfdVersion(Version pVersion = Version::Unknown);
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


} // namespace governikus
