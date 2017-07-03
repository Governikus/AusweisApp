/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "messages/BluetoothMessageResetSimResponse.h"

#include "BluetoothIDs.h"
#include "messages/parameter/BluetoothMessageParameterResultCode.h"

using namespace governikus;


BluetoothMessageResetSimResponse::BluetoothMessageResetSimResponse()
	: BluetoothMessage(BluetoothMsgId::ResetSimResponse)
{
}


BluetoothMessageResetSimResponse::~BluetoothMessageResetSimResponse()
{
}


BluetoothResultCode BluetoothMessageResetSimResponse::getResultCode() const
{
	const auto& param = getParameter(BluetoothParamId::ResultCode);
	return param.staticCast<const BluetoothMessageParameterResultCode>()->getResultCode();
}
