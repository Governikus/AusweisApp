/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "messages/BluetoothMessageConnectResponse.h"

#include "BluetoothIDs.h"
#include "messages/parameter/BluetoothMessageParameterConnectionStatus.h"
#include "messages/parameter/BluetoothMessageParameterMaxMsgSize.h"

using namespace governikus;

BluetoothMessageConnectResponse::BluetoothMessageConnectResponse()
	: BluetoothMessage(BluetoothMsgId::ConnectResponse)
{
}


BluetoothMessageConnectResponse::~BluetoothMessageConnectResponse()
{
}


BluetoothConnectionStatus BluetoothMessageConnectResponse::getConnectionStatus() const
{
	const auto& param = getParameter(BluetoothParamId::ConnectionStatus);
	return param.staticCast<const BluetoothMessageParameterConnectionStatus>()->getResultCode();
}


unsigned int BluetoothMessageConnectResponse::getMaxMsgSize() const
{
	const auto& param = getParameter(BluetoothParamId::MaxMsgSize);
	return param.staticCast<const BluetoothMessageParameterMaxMsgSize>()->getMaxMsgSize();
}
