/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
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

} // namespace governikus
