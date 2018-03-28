/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
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
}


IfdTransmitResponse::IfdTransmitResponse(const QString& pSlotHandle, const QByteArray& pResponseApdu, const QString& pResultMinor)
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
}


const QString& IfdTransmitResponse::getSlotHandle() const
{
	return mSlotHandle;
}


const QByteArray& IfdTransmitResponse::getResponseApdu() const
{
	return mResponseApdu;
}


QJsonDocument IfdTransmitResponse::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	QJsonArray responseApdus;
	responseApdus += QString::fromLatin1(mResponseApdu.toHex());
	result[RESPONSE_APDUS()] = responseApdus;

	return QJsonDocument(result);
}
