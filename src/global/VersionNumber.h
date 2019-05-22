/*!
 * \brief Extension to QVersionNumber.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QVersionNumber>

namespace governikus
{

class VersionNumber
{
	private:
		QVersionNumber mVersionNumber;
		QString mSuffix;

		friend bool operator==(const VersionNumber& pLeft, const VersionNumber& pRight);

	public:
		static const VersionNumber& getApplicationVersion();
		VersionNumber(const QString& pVersion);

		const QVersionNumber& getVersionNumber() const;
		bool isDeveloperVersion() const;
		int getDistance() const;
		QString getBranch() const;
		QString getRevision() const;
		bool isDraft() const;
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


} // namespace governikus
