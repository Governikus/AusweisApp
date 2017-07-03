/*!
 * BluetoothMessage.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BluetoothMessage.h"

using namespace governikus;

static int registerBluetoothMessagePtr = qRegisterMetaType<BluetoothMessage::Ptr>("BluetoothMessage::Ptr");

QDebug operator<<(QDebug pDbg, const governikus::BluetoothMessage& pMsg)
{
	pDbg.nospace() << pMsg.toString();
	return pDbg.space();
}


BluetoothMessage::BluetoothMessage(BluetoothMsgId pMsgId)
	: mMsgId(pMsgId)
	, mMessageParameter()
{
	Q_UNUSED(registerBluetoothMessagePtr);
}


BluetoothMessage::~BluetoothMessage()
{
}


void BluetoothMessage::addParameter(BluetoothMessageParameter::Ptr pMessageParameter)
{
	mMessageParameter.append(pMessageParameter);
}


const BluetoothMessage::ParameterList& BluetoothMessage::getParameterList() const
{
	return mMessageParameter;
}


BluetoothMsgId BluetoothMessage::getBluetoothMsgId() const
{
	return mMsgId;
}


QByteArray BluetoothMessage::toData() const
{
	QByteArray data;
	data.append(static_cast<char>(mMsgId));
	data.append(static_cast<char>(mMessageParameter.size()));
	data.append(static_cast<char>(0x00));
	data.append(static_cast<char>(0x00));

	for (const auto& parameter : getParameterList())
	{
		data.append(parameter->toData());
	}

	return data;
}


QString BluetoothMessage::toString() const
{
	static const QString paramDesc(QStringLiteral(" | Parameter: "));

	QString str = getEnumName(mMsgId);
	for (const auto& parameter : getParameterList())
	{
		str += paramDesc + parameter->toString();
	}
	return str;
}
