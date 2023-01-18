/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */


#include "IfdTransmitResponse.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(RESPONSE_APDU, "ResponseAPDU")
VALUE_NAME(RESPONSE_APDUS, "ResponseAPDUs")
} // namespace


void IfdTransmitResponse::parseResponseApdu(const QJsonObject& pMessageObject)
{
	const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();

	bool responseApduFound = false;
	if (v0Supported && pMessageObject.contains(RESPONSE_APDUS()))
	{
		responseApduFound = true;
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
				qCDebug(ifd) << "Only using the first ResponseAPDU. Command chaining is not supported yet";
			}
		}
		else
		{
			invalidType(RESPONSE_APDUS(), QLatin1String("array"));
		}
	}

	if (!v0Supported || pMessageObject.contains(RESPONSE_APDU()))
	{
		responseApduFound = true;
		const QString& responseApdu = getStringValue(pMessageObject, RESPONSE_APDU());
		mResponseApdu = QByteArray::fromHex(responseApdu.toUtf8());
	}

	if (!responseApduFound)
	{
		missingValue(RESPONSE_APDUS());
		missingValue(RESPONSE_APDU());
	}
}


IfdTransmitResponse::IfdTransmitResponse(const QString& pSlotHandle, const QByteArray& pResponseApdu, ECardApiResult::Minor pResultMinor)
	: IfdMessageResponse(IfdMessageType::IFDTransmitResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
	, mResponseApdu(pResponseApdu)
{
}


IfdTransmitResponse::IfdTransmitResponse(const QJsonObject& pMessageObject)
	: IfdMessageResponse(pMessageObject)
	, mSlotHandle()
	, mResponseApdu()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	parseResponseApdu(pMessageObject);

	if (getType() != IfdMessageType::IFDTransmitResponse)
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


QByteArray IfdTransmitResponse::toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	if (pIfdVersion >= IfdVersion::Version::v2)
	{
		result[RESPONSE_APDU()] = QString::fromLatin1(mResponseApdu.toHex());
	}
	else
	{
		QJsonArray responseApdus;
		responseApdus += QString::fromLatin1(mResponseApdu.toHex());
		result[RESPONSE_APDUS()] = responseApdus;
	}

	return IfdMessage::toByteArray(result);
}
