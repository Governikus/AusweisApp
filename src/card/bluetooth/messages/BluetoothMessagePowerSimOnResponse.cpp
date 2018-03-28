/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothIDs.h"
#include "messages/BluetoothMessagePowerSimOnResponse.h"
#include "messages/parameter/BluetoothMessageParameterResultCode.h"

using namespace governikus;


BluetoothMessagePowerSimOnResponse::BluetoothMessagePowerSimOnResponse()
	: BluetoothMessage(BluetoothMsgId::PowerSimOnResponse)
{
}


BluetoothMessagePowerSimOnResponse::~BluetoothMessagePowerSimOnResponse()
{
}


BluetoothResultCode BluetoothMessagePowerSimOnResponse::getResultCode() const
{
	const auto& param = getParameter(BluetoothParamId::ResultCode);
	return param.staticCast<const BluetoothMessageParameterResultCode>()->getResultCode();
}
