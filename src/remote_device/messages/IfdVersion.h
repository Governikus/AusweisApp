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
#ifndef QT_NO_DEBUG
			, v_test
#endif
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
		static Version fromString(const QString& pVersionString);

		static QVector<Version> supported();
		static Version selectLatestSupported(const QVector<Version>& pVersions);
};


inline QDebug operator<<(QDebug pDbg, const IfdVersion& pVersion)
{
	pDbg << pVersion.toString();
	return pDbg;
}


} // namespace governikus
