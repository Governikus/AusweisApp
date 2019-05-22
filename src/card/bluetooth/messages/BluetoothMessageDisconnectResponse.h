/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/BluetoothMessage.h"


namespace governikus
{

class BluetoothMessageDisconnectResponse
	: public BluetoothMessage
{
	public:
		BluetoothMessageDisconnectResponse();
		virtual ~BluetoothMessageDisconnectResponse();
};

} // namespace governikus
