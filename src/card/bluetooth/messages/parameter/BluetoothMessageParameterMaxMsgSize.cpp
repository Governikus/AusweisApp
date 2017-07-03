/*!
 * BluetoothMessageParameterMaxMsgSize.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
	mMaxMsgSize = pValue.at(1) | (pValue.at(0) << 8);
}


BluetoothMessageParameterMaxMsgSize::BluetoothMessageParameterMaxMsgSize(int pMaxMsgSize)
	: BluetoothMessageParameter(BluetoothParamId::MaxMsgSize, QByteArray())
	, mMaxMsgSize(0)
{
	if (pMaxMsgSize >> 16)
	{
		qCWarning(card) << "MaxMsgSize has wrong length";
		mValid = false;
		return;
	}
	mValue.append((pMaxMsgSize >> 8) % 256);
	mValue.append(pMaxMsgSize % 256);
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
