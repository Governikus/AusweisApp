/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "DeviceError.h"
#include "moc_DeviceError.cpp"

#include "GlobalStatus.h"

namespace governikus
{

static int registerDeviceError = qRegisterMetaType<DeviceError>("DeviceError");

namespace DeviceErrorUtil
{
GlobalStatus toGlobalStatus(DeviceError pDeviceError)
{
	Q_UNUSED(registerDeviceError);

	switch (pDeviceError)
	{
		case DeviceError::DEVICE_CONNECTION_ERROR:
			return GlobalStatus::Code::Workflow_Reader_Device_Connection_Error;

		case DeviceError::DEVICE_POWERED_OFF:
			return GlobalStatus::Code::No_Error;

		case DeviceError::DEVICE_SCAN_ERROR:
			return GlobalStatus::Code::Workflow_Reader_Device_Scan_Error;

		case DeviceError::UNKNOWN_ERROR:
			return GlobalStatus::Code::Unknown_Error;
	}

	Q_UNREACHABLE();
}


}
}
