/*!
 * BluetoothMessageParameterResultCode.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BluetoothMessageParameterResultCode.h"

using namespace governikus;

BluetoothMessageParameterResultCode::BluetoothMessageParameterResultCode(const QByteArray& pValue)
	: BluetoothMessageParameter(BluetoothParamId::ResultCode, pValue)
	, mResultCode()
{
	mValid = parseByteParameter(pValue, mResultCode);
}


BluetoothMessageParameterResultCode::~BluetoothMessageParameterResultCode()
{
}


BluetoothResultCode BluetoothMessageParameterResultCode::getResultCode() const
{
	return mResultCode;
}


QString BluetoothMessageParameterResultCode::toStringValue() const
{
	return getEnumName(mResultCode);
}
