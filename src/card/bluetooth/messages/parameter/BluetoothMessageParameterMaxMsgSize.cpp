/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothMessageParameterMaxMsgSize.h"

using namespace governikus;

BluetoothMessageParameterMaxMsgSize::BluetoothMessageParameterMaxMsgSize(const QByteArray& pValue)
	: BluetoothMessageParameter(BluetoothParamId::MaxMsgSize, pValue)
	, mMaxMsgSize(0)
{
	if (pValue.size() != 2)
	{
		qCWarning(card) << "Content has wrong length";
		mValue.clear();
		mValid = false;
		return;
	}
	mMaxMsgSize = static_cast<uint>(pValue.at(1) | (pValue.at(0) << 8));
}


BluetoothMessageParameterMaxMsgSize::BluetoothMessageParameterMaxMsgSize(uint pMaxMsgSize)
	: BluetoothMessageParameter(BluetoothParamId::MaxMsgSize, QByteArray())
	, mMaxMsgSize(0)
{
	if (pMaxMsgSize >> 16)
	{
		qCWarning(card) << "MaxMsgSize has wrong length";
		mValid = false;
		return;
	}
	mValue += static_cast<char>((pMaxMsgSize >> 8) & 0xFF);
	mValue += static_cast<char>(pMaxMsgSize & 0xFF);
	mMaxMsgSize = pMaxMsgSize;
}


BluetoothMessageParameterMaxMsgSize::~BluetoothMessageParameterMaxMsgSize()
{
}


unsigned int BluetoothMessageParameterMaxMsgSize::getMaxMsgSize() const
{
	return mMaxMsgSize;
}


QString BluetoothMessageParameterMaxMsgSize::toStringValue() const
{
	return QString::number(mMaxMsgSize);
}
