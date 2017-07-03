//
//  BluetoothMessageResetSimResponse.cpp
//  AusweisApp2
//
//  Created by gov on 27.10.14.
//
//

#include "BluetoothIDs.h"
#include "messages/BluetoothMessageResetSimResponse.h"
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
	QSharedPointer<const BluetoothMessageParameterResultCode> param = getParameter<BluetoothMessageParameterResultCode>();
	return param->getResultCode();
}
