/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BluetoothIDs.h"
#include "BluetoothMessage.h"


namespace governikus
{

class BluetoothMessageConnectResponse
	: public BluetoothMessage
{
	private:
		Q_DISABLE_COPY(BluetoothMessageConnectResponse)

	public:
		BluetoothMessageConnectResponse();
		virtual ~BluetoothMessageConnectResponse();

		BluetoothConnectionStatus getConnectionStatus() const;

		unsigned int getMaxMsgSize() const;
};

} /* namespace governikus */
