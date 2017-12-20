/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothMessageParameterConnectionStatus.h"

using namespace governikus;

BluetoothMessageParameterConnectionStatus::BluetoothMessageParameterConnectionStatus(const QByteArray& pValue)
	: BluetoothMessageParameter(BluetoothParamId::ConnectionStatus, pValue)
	, mConnectionStatus()
{
	mValid = parseByteParameter(pValue, mConnectionStatus);
}


BluetoothMessageParameterConnectionStatus::~BluetoothMessageParameterConnectionStatus()
{
}


BluetoothConnectionStatus BluetoothMessageParameterConnectionStatus::getResultCode() const
{
	return mConnectionStatus;
}


QString BluetoothMessageParameterConnectionStatus::toStringValue() const
{
	return getEnumName(mConnectionStatus);
}
