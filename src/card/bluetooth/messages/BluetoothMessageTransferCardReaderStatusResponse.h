/*
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/BluetoothMessage.h"
#include "parameter/BluetoothMessageParameterCardReaderStatus.h"
#include "parameter/BluetoothMessageParameterStatusChange.h"

namespace governikus
{

class BluetoothMessageTransferCardReaderStatusResponse
	: public BluetoothMessage
{
	public:
		BluetoothMessageTransferCardReaderStatusResponse();
		virtual ~BluetoothMessageTransferCardReaderStatusResponse();

		BluetoothStatusChange getStatusChange() const;
};

} /* namespace governikus */
