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
	QSharedPointer<const BluetoothMessageParameterConnectionStatus> param = getParameter<BluetoothMessageParameterConnectionStatus>();
	return param->getResultCode();
}


unsigned int BluetoothMessageConnectResponse::getMaxMsgSize() const
{
	QSharedPointer<const BluetoothMessageParameterMaxMsgSize> param = getParameter<BluetoothMessageParameterMaxMsgSize>();
	return param->getMaxMsgSize();
}
