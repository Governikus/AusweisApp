/*!
 * BluetoothMessageParameterApduResponse.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BluetoothMessageParameterApduResponse.h"

using namespace governikus;

BluetoothMessageParameterApduResponse::BluetoothMessageParameterApduResponse(const QByteArray& pApdu)
	: BluetoothMessageParameter(BluetoothParamId::ResponseAPDU, pApdu)
{
}


BluetoothMessageParameterApduResponse::~BluetoothMessageParameterApduResponse()
{
}


const QByteArray& BluetoothMessageParameterApduResponse::getResponseApdu() const
{
	return getValue();
}


QString BluetoothMessageParameterApduResponse::toStringValue() const
{
	const QString responseApdu = QString::fromLatin1(getResponseApdu().toHex());
	return QStringLiteral("(ResponseApdu: %1)").arg(responseApdu);
}
