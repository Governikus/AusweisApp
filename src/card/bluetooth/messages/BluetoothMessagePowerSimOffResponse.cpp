//
//  BluetoothMessagePowerSimOffResponse.cpp
//  AusweisApp2
//
//  Created by gov on 27.10.14.
//
//

#include "BluetoothIDs.h"
#include "messages/BluetoothMessagePowerSimOffResponse.h"
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
	QSharedPointer<const BluetoothMessageParameterResultCode> param = getParameter<BluetoothMessageParameterResultCode>();
	return param->getResultCode();
}
