/*!
 * \brief Implements a wrapper for different APIs to get
 * device information like android device name.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

class DeviceInfo
{
	private:
		DeviceInfo();
		~DeviceInfo();
		Q_DISABLE_COPY(DeviceInfo)

#ifdef Q_OS_ANDROID
		static QString getField(const char* pField);
#endif

	public:
		static QString getPrettyInfo();
		static QString getName();
		static QString getFingerprint();
};

} /* namespace governikus */
