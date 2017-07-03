/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "messages/BluetoothMessagePowerSimOffResponse.h"

#include "BluetoothIDs.h"
#include "messages/parameter/BluetoothMessageParameterResultCode.h"

using namespace governikus;


BluetoothMessagePowerSimOffResponse::BluetoothMessagePowerSimOffResponse()
	: BluetoothMessage(BluetoothMsgId::PowerSimOffResponse)
{
}


BluetoothMessagePowerSimOffResponse::~BluetoothMessagePowerSimOffResponse()
{
}


BluetoothResultCode BluetoothMessagePowerSimOffResponse::getResultCode() const
{
	const auto& param = getParameter(BluetoothParamId::ResultCode);
	return param.staticCast<const BluetoothMessageParameterResultCode>()->getResultCode();
}
