/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothMessage.h"

#include "Initializer.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(bluetooth)

using namespace governikus;

static Initializer::Entry X([] {
			qRegisterMetaType<BluetoothMessage::Ptr>("BluetoothMessage::Ptr");
		});


QDebug operator<<(QDebug pDbg, const governikus::BluetoothMessage& pMsg)
{
	pDbg.nospace() << pMsg.toString();
	return pDbg.space();
}


BluetoothMessage::BluetoothMessage(BluetoothMsgId pMsgId)
	: mMsgId(pMsgId)
	, mMessageParameter()
{
}


BluetoothMessage::~BluetoothMessage()
{
}


BluetoothMessageParameter::Ptr BluetoothMessage::getParameter(BluetoothParamId pId) const
{
	return mMessageParameter.value(pId);
}


void BluetoothMessage::addParameter(BluetoothMessageParameter::Ptr pMessageParameter)
{
	Q_ASSERT(!pMessageParameter.isNull());

	if (mMessageParameter.contains(pMessageParameter->getParameterId()))
	{
		qCWarning(bluetooth) << "Parameter ID is already added:" << pMessageParameter->getParameterId();
	}

	mMessageParameter.insertMulti(pMessageParameter->getParameterId(), pMessageParameter);
}


BluetoothMsgId BluetoothMessage::getBluetoothMsgId() const
{
	return mMsgId;
}


QByteArray BluetoothMessage::toData() const
{
	QByteArray data;
	data += Enum<BluetoothMsgId>::getValue(mMsgId);
	if (mMessageParameter.size() > CHAR_MAX)
	{
		qCCritical(bluetooth) << "Message parameter count > CHAR_MAX not supported";
		Q_ASSERT(mMessageParameter.size() <= CHAR_MAX);
		return QByteArray();
	}
	data += static_cast<char>(mMessageParameter.size());
	data += '\0';
	data += '\0';

	for (const auto& parameter : qAsConst(mMessageParameter))
	{
		data += parameter->toData();
	}

	return data;
}


QString BluetoothMessage::toString() const
{
	static const QString paramDesc(QStringLiteral(" | Parameter: "));

	QString str = getEnumName(mMsgId);
	for (const auto& parameter : qAsConst(mMessageParameter))
	{
		str += paramDesc + parameter->toString();
	}
	return str;
}
