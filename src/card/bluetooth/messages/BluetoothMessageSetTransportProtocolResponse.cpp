/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BluetoothMessageSetTransportProtocolResponse.h"

#include "parameter/BluetoothMessageParameterResultCode.h"

using namespace governikus;


BluetoothMessageSetTransportProtocolResponse::BluetoothMessageSetTransportProtocolResponse()
	: BluetoothMessage(BluetoothMsgId::SetTransportProtocolResponse)
{
}


BluetoothMessageSetTransportProtocolResponse::~BluetoothMessageSetTransportProtocolResponse()
{
}


BluetoothResultCode BluetoothMessageSetTransportProtocolResponse::getResultCode() const
{
	const auto& param = getParameter(BluetoothParamId::ResultCode);
	return param.staticCast<const BluetoothMessageParameterResultCode>()->getResultCode();
}
