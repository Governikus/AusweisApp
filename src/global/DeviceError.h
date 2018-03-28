/*!
 * \brief Global definitions for device error codes.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{

class GlobalStatus;


defineEnumType(DeviceError,
		UNKNOWN_ERROR,
		DEVICE_CONNECTION_ERROR,
		DEVICE_POWERED_OFF,
		DEVICE_SCAN_ERROR)

namespace DeviceErrorUtil
{
GlobalStatus toGlobalStatus(DeviceError pDeviceError);
} /* namespace DeviceErrorUtil */

} /* namespace governikus */
