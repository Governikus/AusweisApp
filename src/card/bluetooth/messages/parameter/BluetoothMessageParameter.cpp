/*!
 * BluetoothMessageParameter.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BluetoothMessageParameter.h"
#include "messages/BluetoothUtils.h"

using namespace governikus;

QDebug operator<<(QDebug pDbg, const BluetoothMessageParameter& pMsg)
{
	pDbg.nospace() << pMsg.toString();
	return pDbg.space();
}


BluetoothMessageParameter::BluetoothMessageParameter(BluetoothParamId pParamId, const QByteArray& pValue)
	: mParamId(pParamId)
	, mValue(pValue)
	, mValid(true)
{
}


BluetoothMessageParameter::~BluetoothMessageParameter()
{
}


BluetoothParamId BluetoothMessageParameter::getParameterId() const
{
	return mParamId;
}


const QByteArray& BluetoothMessageParameter::getValue() const
{
	return mValue;
}


QString BluetoothMessageParameter::toStringValue() const
{
	return QString::fromLatin1(mValue.toHex());
}


QString BluetoothMessageParameter::toString() const
{
	return toStringBase() + toStringValue();
}


bool BluetoothMessageParameter::isValid() const
{
	return mValid;
}


QByteArray BluetoothMessageParameter::toData() const
{
	QByteArray data;
	data.append(static_cast<char>(mParamId));
	data.append(static_cast<char>(0x00));
	data.append((mValue.size() & 0xFF00) >> 8);
	data.append(mValue.size() & 0x00FF);

	data.append(mValue);
	BluetoothUtils::addPadding(data, mValue);
	return data;
}
