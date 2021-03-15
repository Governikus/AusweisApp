/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
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
VALUE_NAME(DISPLAY_TEXT, "DisplayText")
VALUE_NAME(ACCEPTABLE_STATUS_CODES, "AcceptableStatusCodes")
} // namespace


void IfdTransmit::parseInputApdu(const QJsonObject& pMessageObject)
{
	const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();

	bool inputApduFound = false;
	if (v0Supported && pMessageObject.contains(COMMAND_APDUS()))
	{
		inputApduFound = true;
		const auto& value = pMessageObject.value(COMMAND_APDUS());
		if (value.isArray())
		{
			const auto& entry = value.toArray().at(0);
			if (entry.isObject())
			{
				const QJsonObject& commandApdu = entry.toObject();
				const QString& inputApdu = getStringValue(commandApdu, INPUT_APDU());
				mInputApdu = QByteArray::fromHex(inputApdu.toUtf8());
			}
			else
			{
				invalidType(COMMAND_APDUS(), QLatin1String("object array"));
			}

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

	if (!v0Supported || pMessageObject.contains(INPUT_APDU()))
	{
		inputApduFound = true;
		const QString& inputApdu = getStringValue(pMessageObject, INPUT_APDU());
		mInputApdu = QByteArray::fromHex(inputApdu.toUtf8());
	}

	if (!inputApduFound)
	{
		missingValue(COMMAND_APDUS());
		missingValue(INPUT_APDU());
	}
}


IfdTransmit::IfdTransmit(const QString& pSlotHandle, const QByteArray& pInputApdu, const QString& pDisplayText)
	: RemoteMessage(RemoteCardMessageType::IFDTransmit)
	, mSlotHandle(pSlotHandle)
	, mInputApdu(pInputApdu)
	, mDisplayText(pDisplayText)
{
}


IfdTransmit::IfdTransmit(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mSlotHandle()
	, mInputApdu()
	, mDisplayText()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	parseInputApdu(pMessageObject);

	if (pMessageObject.contains(DISPLAY_TEXT()))
	{
		mDisplayText = getStringValue(pMessageObject, DISPLAY_TEXT());
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


const QString& IfdTransmit::getDisplayText() const
{
	return mDisplayText;
}


QByteArray IfdTransmit::toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	if (pIfdVersion.getVersion() >= IfdVersion::Version::v2)
	{
		result[INPUT_APDU()] = QString::fromLatin1(mInputApdu.toHex());
		if (!mDisplayText.isNull())
		{
			result[DISPLAY_TEXT()] = mDisplayText;
		}
	}
	else
	{
		QJsonArray commandApdus;
		QJsonObject commandApdu;
		commandApdu[INPUT_APDU()] = QString::fromLatin1(mInputApdu.toHex());
		commandApdu[ACCEPTABLE_STATUS_CODES()] = QJsonValue();
		commandApdus += commandApdu;
		result[COMMAND_APDUS()] = commandApdus;
	}

	return RemoteMessage::toByteArray(result);
}
