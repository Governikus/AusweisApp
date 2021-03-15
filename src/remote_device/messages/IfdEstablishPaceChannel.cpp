/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "IfdEstablishPaceChannel.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(INPUT_DATA, "InputData")
VALUE_NAME(PREFERRED_PIN_LENGTH, "PreferredPinLength")
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


IfdEstablishPaceChannel::IfdEstablishPaceChannel(const QString& pSlotHandle, const EstablishPaceChannel& pInputData, int pPreferredPinLength)
	: RemoteMessage(RemoteCardMessageType::IFDEstablishPACEChannel)
	, mSlotHandle(pSlotHandle)
	, mInputData(pInputData)
	, mPreferredPinLength(pPreferredPinLength)
{
}


IfdEstablishPaceChannel::IfdEstablishPaceChannel(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mSlotHandle()
	, mInputData()
	, mPreferredPinLength(0)
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	parseInputData(pMessageObject);

	if (pMessageObject.contains(PREFERRED_PIN_LENGTH()))
	{
		mPreferredPinLength = getIntValue(pMessageObject, PREFERRED_PIN_LENGTH(), 0);
	}

	if (getType() != RemoteCardMessageType::IFDEstablishPACEChannel)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDEstablishPACEChannel"));
	}
}


const QString& IfdEstablishPaceChannel::getSlotHandle() const
{
	return mSlotHandle;
}


const EstablishPaceChannel& IfdEstablishPaceChannel::getInputData() const
{
	return mInputData;
}


int IfdEstablishPaceChannel::getPreferredPinLength() const
{
	return mPreferredPinLength;
}


QByteArray IfdEstablishPaceChannel::toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	if (pIfdVersion.getVersion() >= IfdVersion::Version::v2)
	{
		result[INPUT_DATA()] = QString::fromLatin1(mInputData.createInputData().toHex());
		if (mPreferredPinLength > 0)
		{
			result[PREFERRED_PIN_LENGTH()] = mPreferredPinLength;
		}
	}
	else
	{
		result[INPUT_DATA()] = QString::fromLatin1(mInputData.createCommandDataCcid().toHex());
	}

	return RemoteMessage::toByteArray(result);
}
