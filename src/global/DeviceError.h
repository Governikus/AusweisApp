/*!
 * DeviceError.h
 *
 * \brief Global definitions for device error codes.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"
#include "GlobalStatus.h"

namespace governikus
{

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
