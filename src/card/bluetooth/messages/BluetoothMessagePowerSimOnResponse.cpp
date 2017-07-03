/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
	QSharedPointer<const BluetoothMessageParameterResultCode> param = getParameter<BluetoothMessageParameterResultCode>();
	return param->getResultCode();
}
