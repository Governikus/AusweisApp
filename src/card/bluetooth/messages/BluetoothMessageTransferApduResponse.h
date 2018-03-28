/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Apdu.h"
#include "BluetoothIDs.h"
#include "messages/BluetoothMessage.h"


namespace governikus
{

class BluetoothMessageTransferApduResponse
	: public BluetoothMessage
{
	public:
		BluetoothMessageTransferApduResponse();
		virtual ~BluetoothMessageTransferApduResponse();

		BluetoothResultCode getResultCode() const;
		bool hasResponseAPDU() const;
		const QByteArray& getResponseAPDU() const;
};

} /* namespace governikus */
