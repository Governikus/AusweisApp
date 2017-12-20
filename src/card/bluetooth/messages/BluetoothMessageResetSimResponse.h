/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BluetoothMessage.h"


namespace governikus
{

class BluetoothMessageResetSimResponse
	: public BluetoothMessage
{
	private:
		Q_DISABLE_COPY(BluetoothMessageResetSimResponse)

	public:
		BluetoothMessageResetSimResponse();
		virtual ~BluetoothMessageResetSimResponse();

		BluetoothResultCode getResultCode() const;
};

} /* namespace governikus */
