/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "BluetoothMessage.h"

namespace governikus
{

class BluetoothMessagePowerSimOffResponse
	: public BluetoothMessage
{
	private:
		Q_DISABLE_COPY(BluetoothMessagePowerSimOffResponse)

	public:
		BluetoothMessagePowerSimOffResponse();
		virtual ~BluetoothMessagePowerSimOffResponse();

		BluetoothResultCode getResultCode() const;
};

} /* namespace governikus */
