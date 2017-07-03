/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "messages/BluetoothMessageDisconnectResponse.h"

using namespace governikus;

BluetoothMessageDisconnectResponse::BluetoothMessageDisconnectResponse()
	: BluetoothMessage(BluetoothMsgId::DisconnectResponse)
{
}


BluetoothMessageDisconnectResponse::~BluetoothMessageDisconnectResponse()
{
}
