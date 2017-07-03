/*
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "messages/parameter/BluetoothMessageParameterCardReaderStatus.h"

using namespace governikus;

BluetoothMessageParameterCardReaderStatus::BluetoothMessageParameterCardReaderStatus(const QByteArray& pValue)
	: BluetoothMessageParameter(BluetoothParamId::CardReaderStatus, pValue)
	, mCardReaderStatus(BluetoothCardReaderStatus::Unknown)
{
	mValid = parseByteParameter(pValue, mCardReaderStatus);
}


BluetoothMessageParameterCardReaderStatus::~BluetoothMessageParameterCardReaderStatus()
{
}


BluetoothStatusChange BluetoothMessageParameterCardReaderStatus::getStatusChange() const
{
	if (mCardReaderStatus == BluetoothCardReaderStatus::CardInserted)
	{
		return BluetoothStatusChange::CardInserted;
	}
	else if (mCardReaderStatus == BluetoothCardReaderStatus::CardRemoved)
	{
		return BluetoothStatusChange::CardRemoved;
	}

	return BluetoothStatusChange::Unknown;
}


BluetoothCardReaderStatus BluetoothMessageParameterCardReaderStatus::getCardReaderStatus() const
{
	return mCardReaderStatus;
}


QString BluetoothMessageParameterCardReaderStatus::toStringValue() const
{
	return getEnumName(mCardReaderStatus);
}
