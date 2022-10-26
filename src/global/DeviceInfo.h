/*!
 * \brief Implements a wrapper for different APIs to get
 * device information like android device name.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

class DeviceInfo
{
	Q_DISABLE_COPY(DeviceInfo)

	private:
		DeviceInfo() = default;
		~DeviceInfo() = default;

#if defined(Q_OS_ANDROID)
		static QString getField(const char* const pField);
#endif

#if defined(Q_OS_IOS)
		static QString getMachineId();
		static QString getBuildNumber();
#endif

	public:
		[[nodiscard]] static QString getPrettyInfo();
		[[nodiscard]] static QString getName();
		[[nodiscard]] static QString getFingerprint();
		[[nodiscard]] static QString getOSBuildNumber();
		[[nodiscard]] static QString getOSVersion();
		[[nodiscard]] static QString getKernelVersion();
		[[nodiscard]] static QString getVendor();
		[[nodiscard]] static QString getModelNumber();
		[[nodiscard]] static QString getModelName();
};

} // namespace governikus
