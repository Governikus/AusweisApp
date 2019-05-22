/*!
 * \brief Class holding information about an Android device to be sent to
 * the whitelist server.
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{
class Survey
{
	private:
		const bool mNull;
		const QString mBuildNumber;
		const QString mAndroidVersion;
		const QString mKernelVersion;
		const int mMaximumNfcPacketLength;
		const QString mVendor;
		const QString mModelNumber;
		const QString mModelName;
		const QString mAusweisAppVersionNumber;

	public:
		Survey(const QString& pBuildNumber,
				const QString& pAndroidVersion,
				const QString& pKernelVersion,
				int pMaximumNfcPacketLength,
				const QString& pVendor,
				const QString& pModelNumber,
				const QString& pModelName,
				const QString& pAusweisAppVersionNumber);

		Survey();

		QByteArray toJsonByteArray() const;
		bool isNull() const;
};

} // namespace governikus
