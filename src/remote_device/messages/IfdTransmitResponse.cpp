/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */


#include "IfdTransmitResponse.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(RESPONSE_APDUS, "ResponseAPDUs")
} // namespace


IfdTransmitResponse::IfdTransmitResponse(const QString& pSlotHandle, const QByteArray& pResponseApdu, ECardApiResult::Minor pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDTransmitResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
	, mResponseApdu(pResponseApdu)
{
}


IfdTransmitResponse::IfdTransmitResponse(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mSlotHandle()
	, mResponseApdu()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	if (pMessageObject.contains(RESPONSE_APDUS()))
	{
		const auto& value = pMessageObject.value(RESPONSE_APDUS());
		if (value.isArray())
		{
			const auto& inputApduValue = value.toArray().at(0);
			if (inputApduValue.isString())
			{
				mResponseApdu = QByteArray::fromHex(inputApduValue.toString().toUtf8());
			}
			else
			{
				invalidType(RESPONSE_APDUS(), QLatin1String("string array"));
			}
			if (value.toArray().size() > 1)
			{
				qCDebug(remote_device) << "Only using the first ResponseAPDU. Command chaining ist not supported yet";
			}
		}
		else
		{
			invalidType(RESPONSE_APDUS(), QLatin1String("array"));
		}
	}
	else
	{
		missingValue(RESPONSE_APDUS());
	}

	if (getType() != RemoteCardMessageType::IFDTransmitResponse)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDTransmitResponse"));
	}
}


const QString& IfdTransmitResponse::getSlotHandle() const
{
	return mSlotHandle;
}


const QByteArray& IfdTransmitResponse::getResponseApdu() const
{
	return mResponseApdu;
}


QByteArray IfdTransmitResponse::toByteArray(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	QJsonArray responseApdus;
	responseApdus += QString::fromLatin1(mResponseApdu.toHex());
	result[RESPONSE_APDUS()] = responseApdus;

	return RemoteMessage::toByteArray(result);
}
