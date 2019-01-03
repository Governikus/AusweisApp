/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "IfdTransmit.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(COMMAND_APDUS, "CommandAPDUs")
VALUE_NAME(INPUT_APDU, "InputAPDU")
VALUE_NAME(ACCEPTABLE_STATUS_CODES, "AcceptableStatusCodes")
} // namespace


void IfdTransmit::parseCommandApdu(QJsonValue pEntry)
{
	if (!pEntry.isObject())
	{
		invalidType(COMMAND_APDUS(), QLatin1String("object array"));
		return;
	}

	const QJsonObject& commandApdu = pEntry.toObject();
	const QString& inputApdu = getStringValue(commandApdu, INPUT_APDU());
	mInputApdu = QByteArray::fromHex(inputApdu.toUtf8());
}


IfdTransmit::IfdTransmit(const QString& pSlotHandle, const QByteArray& pInputApdu)
	: RemoteMessage(RemoteCardMessageType::IFDTransmit)
	, mSlotHandle(pSlotHandle)
	, mInputApdu(pInputApdu)
{
}


IfdTransmit::IfdTransmit(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mSlotHandle()
	, mInputApdu()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	if (pMessageObject.contains(COMMAND_APDUS()))
	{
		const auto& value = pMessageObject.value(COMMAND_APDUS());
		if (value.isArray())
		{
			parseCommandApdu(value.toArray().at(0));
			if (value.toArray().size() > 1)
			{
				qCDebug(remote_device) << "Only using the first CommandAPDU. Command chaining ist not supported yet";
			}
		}
		else
		{
			invalidType(COMMAND_APDUS(), QLatin1String("array"));
		}
	}
	else
	{
		missingValue(COMMAND_APDUS());
	}

	if (getType() != RemoteCardMessageType::IFDTransmit)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDTransmit"));
	}
}


const QString& IfdTransmit::getSlotHandle() const
{
	return mSlotHandle;
}


const QByteArray& IfdTransmit::getInputApdu() const
{
	return mInputApdu;
}


QByteArray IfdTransmit::toByteArray(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	QJsonArray commandApdus;
	QJsonObject commandApdu;
	commandApdu[INPUT_APDU()] = QString::fromLatin1(mInputApdu.toHex());
	commandApdu[ACCEPTABLE_STATUS_CODES()] = QJsonValue();
	commandApdus += commandApdu;
	result[COMMAND_APDUS()] = commandApdus;

	return RemoteMessage::toByteArray(result);
}
