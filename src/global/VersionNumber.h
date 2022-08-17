/*!
 * \brief Extension to QVersionNumber.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDebug>
#include <QVersionNumber>

namespace governikus
{

class VersionNumber
{
	friend bool operator==(const VersionNumber& pLeft, const VersionNumber& pRight);

	private:
		QVersionNumber mVersionNumber;
		QString mSuffix;

		[[nodiscard]] auto getInfoFromSuffix(QLatin1Char pStart, QLatin1Char pEnd = QLatin1Char('-')) const;

	public:
		static const VersionNumber& getApplicationVersion();
		explicit VersionNumber(const QString& pVersion);

		[[nodiscard]] const QVersionNumber& getVersionNumber() const;
		[[nodiscard]] bool isDeveloperVersion() const;
		[[nodiscard]] bool isBetaVersion() const;
		[[nodiscard]] int getDistance() const;
		[[nodiscard]] QString getBranch() const;
		[[nodiscard]] QString getRevision() const;
		[[nodiscard]] bool isDraft() const;
		[[nodiscard]] QString getSuffix() const;
};


inline bool operator==(const VersionNumber& pLeft, const VersionNumber& pRight)
{
	return &pLeft == &pRight || (
		pLeft.mVersionNumber == pRight.mVersionNumber &&
		pLeft.mSuffix == pRight.mSuffix);
}


inline bool operator!=(const VersionNumber& pLeft, const VersionNumber& pRight)
{
	return !(pLeft == pRight);
}


inline bool operator<(const VersionNumber& pLeft, const VersionNumber& pRight)
{
	return pLeft.getVersionNumber() < pRight.getVersionNumber() ||
		   (pLeft.getVersionNumber() == pRight.getVersionNumber() && pLeft.getDistance() < pRight.getDistance());
}


inline bool operator>(const VersionNumber& pLeft, const VersionNumber& pRight)
{
	return pLeft.getVersionNumber() > pRight.getVersionNumber() ||
		   (pLeft.getVersionNumber() == pRight.getVersionNumber() && pLeft.getDistance() > pRight.getDistance());
}


inline QDebug operator<<(QDebug pDbg, const VersionNumber& pVersion)
{
	QDebugStateSaver saver(pDbg);
	pDbg.noquote().nospace() << pVersion.getVersionNumber().toString() << pVersion.getSuffix();
	return pDbg;
}


} // namespace governikus
