/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
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
	const auto& param = getParameter(BluetoothParamId::ResultCode);
	return param.staticCast<const BluetoothMessageParameterResultCode>()->getResultCode();
}


bool BluetoothMessageTransferApduResponse::hasResponseAPDU() const
{
	return getParameter(BluetoothParamId::ResponseAPDU);
}


const QByteArray& BluetoothMessageTransferApduResponse::getResponseAPDU() const
{
	const auto& param = getParameter(BluetoothParamId::ResponseAPDU);
	return param.staticCast<const BluetoothMessageParameterApduResponse>()->getResponseApdu();
}
