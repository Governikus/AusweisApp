/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
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
			v0
		};

	private:
		const Version mVersion;

	public:
		IfdVersion(Version pVersion);
		IfdVersion(const QString& pVersionString);

		Version getVersion() const;
		bool isValid() const;
		bool isSupported() const;

		bool operator==(const IfdVersion& pOther) const;
		bool operator!=(const IfdVersion& pOther) const;

		QString toString() const;
		static IfdVersion fromString(const QString& pVersionString);

		static IfdVersion latest();
		static QVector<Version> supported();
		static IfdVersion selectLatestSupported(const QVector<Version>& pVersions);
};


inline QDebug operator<<(QDebug pDbg, const IfdVersion& pVersion)
{
	pDbg << pVersion.toString();
	return pDbg;
}


} // namespace governikus
