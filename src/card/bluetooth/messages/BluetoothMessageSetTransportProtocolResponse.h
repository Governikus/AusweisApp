/*!
 * \brief Store information of SetTransportProtocolResponse.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "messages/BluetoothMessage.h"

namespace governikus
{

class BluetoothMessageSetTransportProtocolResponse
	: public BluetoothMessage
{
	private:
		Q_DISABLE_COPY(BluetoothMessageSetTransportProtocolResponse)

	public:
		BluetoothMessageSetTransportProtocolResponse();
		virtual ~BluetoothMessageSetTransportProtocolResponse();

		BluetoothResultCode getResultCode() const;
};

} /* namespace governikus */
