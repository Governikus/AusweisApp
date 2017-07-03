/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "BluetoothMessage.h"


namespace governikus
{

class BluetoothMessagePowerSimOnResponse
	: public BluetoothMessage
{
	private:
		Q_DISABLE_COPY(BluetoothMessagePowerSimOnResponse)

	public:
		BluetoothMessagePowerSimOnResponse();
		virtual ~BluetoothMessagePowerSimOnResponse();

		BluetoothResultCode getResultCode() const;
};

} /* namespace governikus */
