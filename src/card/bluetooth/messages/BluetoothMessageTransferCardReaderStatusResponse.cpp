/*
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BluetoothMessageTransferCardReaderStatusResponse.h"

using namespace governikus;

BluetoothMessageTransferCardReaderStatusResponse::BluetoothMessageTransferCardReaderStatusResponse()
	: BluetoothMessage(BluetoothMsgId::TransferCardReaderStatusResponse)
{
}


BluetoothMessageTransferCardReaderStatusResponse::~BluetoothMessageTransferCardReaderStatusResponse()
{
}


BluetoothStatusChange BluetoothMessageTransferCardReaderStatusResponse::getStatusChange() const
{
	auto ptr = getParameter<BluetoothMessageParameterCardReaderStatus>();
	if (ptr)
	{
		return ptr->getStatusChange();
	}

	Q_ASSERT(false); // check for isValid before you call this
	return BluetoothStatusChange::Unknown;
}
