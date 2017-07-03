/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "messages/BluetoothMessageTransferApduResponse.h"
#include "messages/parameter/BluetoothMessageParameterApduResponse.h"
#include "messages/parameter/BluetoothMessageParameterResultCode.h"

using namespace governikus;

BluetoothMessageTransferApduResponse::BluetoothMessageTransferApduResponse()
	: BluetoothMessage(BluetoothMsgId::TransferApduResponse)
{
}


BluetoothMessageTransferApduResponse::~BluetoothMessageTransferApduResponse()
{
}


BluetoothResultCode BluetoothMessageTransferApduResponse::getResultCode() const
{
	QSharedPointer<const BluetoothMessageParameterResultCode> param = getParameter<BluetoothMessageParameterResultCode>();
	return param->getResultCode();
}


bool BluetoothMessageTransferApduResponse::hasResponseAPDU() const
{
	QSharedPointer<const BluetoothMessageParameterApduResponse> param = getParameter<BluetoothMessageParameterApduResponse>();
	return param;
}


const QByteArray& BluetoothMessageTransferApduResponse::getResponseAPDU() const
{
	QSharedPointer<const BluetoothMessageParameterApduResponse> param = getParameter<BluetoothMessageParameterApduResponse>();
	return param->getResponseApdu();
}
