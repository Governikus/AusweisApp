/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothMessageParameterStatusChange.h"

using namespace governikus;

BluetoothMessageParameterStatusChange::BluetoothMessageParameterStatusChange(const QByteArray& pValue)
	: BluetoothMessageParameter(BluetoothParamId::StatusChange, pValue)
	, mStatusChange(BluetoothStatusChange::Unknown)
{
	mValid = parseByteParameter(pValue, mStatusChange);
}


BluetoothMessageParameterStatusChange::~BluetoothMessageParameterStatusChange()
{
}


BluetoothStatusChange BluetoothMessageParameterStatusChange::getStatusChange() const
{
	return mStatusChange;
}


QString BluetoothMessageParameterStatusChange::toStringValue() const
{
	return getEnumName(mStatusChange);
}
