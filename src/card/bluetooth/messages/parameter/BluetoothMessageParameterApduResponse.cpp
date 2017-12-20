/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
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
