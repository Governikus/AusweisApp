/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
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
