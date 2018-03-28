/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothMessageParameter.h"
#include "messages/BluetoothUtils.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(bluetooth)

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
	data += Enum<BluetoothParamId>::getValue(mParamId);
	data += '\0';
	if (mValue.size() > 0xFFFF)
	{
		qCCritical(bluetooth) << "Value of BluetoothMessageParameter of size > 0xFFFF not supported";
		Q_ASSERT(mValue.size() <= 0xFFFF);
		return QByteArray();
	}
	data += static_cast<char>((mValue.size() >> 8) & 0xFF);
	data += static_cast<char>(mValue.size() & 0xFF);

	data += mValue;
	BluetoothUtils::addPadding(data, mValue);
	return data;
}
