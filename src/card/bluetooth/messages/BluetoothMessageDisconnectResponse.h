/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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

} /* namespace governikus */
