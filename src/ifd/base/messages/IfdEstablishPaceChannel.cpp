/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdEstablishPaceChannel.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(INPUT_DATA, "InputData")
VALUE_NAME(EXPECTED_PIN_LENGTH, "ExpectedPINLength")
} // namespace


void IfdEstablishPaceChannel::parseInputData(const QJsonObject& pMessageObject)
{
	const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();

	const QString& inputData = getStringValue(pMessageObject, INPUT_DATA());
	if (isIncomplete())
	{
		return;
	}

	const auto& input = QByteArray::fromHex(inputData.toUtf8());
	if (v0Supported && EstablishPaceChannel::isCcid(input))
	{
		if (!mInputData.fromCcid(input))
		{
			markIncomplete(QStringLiteral("The value of InputData should be as defined in TR-03119 section D.3"));
		}

		return;
	}

	if (!mInputData.fromInputData(input))
	{
		if (v0Supported)
		{
			markIncomplete(QStringLiteral("(v0) The value of InputData should be as defined in TR-03119 section D.3"));
			markIncomplete(QStringLiteral("(v2) The value of InputData should be as defined in PC/SC Part 10 AMD1 section 2.6.16"));
		}
		else
		{
			markIncomplete(QStringLiteral("The value of InputData should be as defined in PC/SC Part 10 AMD1 section 2.6.16"));
		}
	}
}


IfdEstablishPaceChannel::IfdEstablishPaceChannel(const QString& pSlotHandle, const EstablishPaceChannel& pInputData, int pExpectedPinLength)
	: IfdMessage(IfdMessageType::IFDEstablishPACEChannel)
	, mSlotHandle(pSlotHandle)
	, mInputData(pInputData)
	, mExpectedPinLength(pExpectedPinLength)
{
}


IfdEstablishPaceChannel::IfdEstablishPaceChannel(const QJsonObject& pMessageObject)
	: IfdMessage(pMessageObject)
	, mSlotHandle()
	, mInputData()
	, mExpectedPinLength(0)
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	parseInputData(pMessageObject);

	if (pMessageObject.contains(EXPECTED_PIN_LENGTH()))
	{
		mExpectedPinLength = getIntValue(pMessageObject, EXPECTED_PIN_LENGTH(), 0);
	}

	ensureType(IfdMessageType::IFDEstablishPACEChannel);
}


const QString& IfdEstablishPaceChannel::getSlotHandle() const
{
	return mSlotHandle;
}


const EstablishPaceChannel& IfdEstablishPaceChannel::getInputData() const
{
	return mInputData;
}


int IfdEstablishPaceChannel::getExpectedPinLength() const
{
	return mExpectedPinLength;
}


QByteArray IfdEstablishPaceChannel::toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	if (pIfdVersion >= IfdVersion::Version::v2)
	{
		result[INPUT_DATA()] = QString::fromLatin1(mInputData.createInputData().toHex());
		if (mExpectedPinLength > 0)
		{
			result[EXPECTED_PIN_LENGTH()] = mExpectedPinLength;
		}
	}
	else
	{
		result[INPUT_DATA()] = QString::fromLatin1(mInputData.createASN1StructCcid().toHex());
	}

	return IfdMessage::toByteArray(result);
}
