/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */


#include "IfdStatus.h"

#include "AppSettings.h"
#include "VolatileSettings.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_NAME, "SlotName")
VALUE_NAME(PIN_CAPABILITIES, "PINCapabilities")
VALUE_NAME(PIN_CAP_PACE, "PACE")
VALUE_NAME(PIN_CAP_EID, "eID")
VALUE_NAME(PIN_CAP_ESIGN, "eSign")
VALUE_NAME(PIN_CAP_DESTROY, "Destroy")
VALUE_NAME(PIN_PAD, "PINPad")
VALUE_NAME(MAX_APDU_LENGTH, "MaxAPDULength")
VALUE_NAME(CONNECTED_READER, "ConnectedReader")
VALUE_NAME(CARD_AVAILABLE, "CardAvailable")
VALUE_NAME(EF_ATR, "EFATR")
VALUE_NAME(EF_DIR, "EFDIR")
} // namespace


QJsonValue IfdStatus::createPaceCapabilities() const
{
	QJsonObject result;
	result[PIN_CAP_PACE()] = mHasPinPad;
	result[PIN_CAP_EID()] = false;
	result[PIN_CAP_ESIGN()] = false;
	result[PIN_CAP_DESTROY()] = false;
	return result;
}


void IfdStatus::parsePinPad(const QJsonObject& pMessageObject)
{
	const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();

	bool pinPadFound = false;
	if (v0Supported && pMessageObject.contains(PIN_CAPABILITIES()))
	{
		pinPadFound = true;
		QJsonValue value = pMessageObject.value(PIN_CAPABILITIES());
		if (value.isObject())
		{
			QJsonObject object = value.toObject();
			mHasPinPad = getBoolValue(object, PIN_CAP_PACE());
			Q_UNUSED(getBoolValue(object, PIN_CAP_EID()))
			Q_UNUSED(getBoolValue(object, PIN_CAP_ESIGN()))
			Q_UNUSED(getBoolValue(object, PIN_CAP_DESTROY()))
		}
		else
		{
			invalidType(PIN_CAPABILITIES(), QLatin1String("object"));
		}
	}

	if (!v0Supported || pMessageObject.contains(PIN_PAD()))
	{
		pinPadFound = true;
		mHasPinPad = getBoolValue(pMessageObject, PIN_PAD());
	}

	if (!pinPadFound)
	{
		missingValue(PIN_CAPABILITIES());
		missingValue(PIN_PAD());
	}
}


IfdStatus::IfdStatus(const ReaderInfo& pReaderInfo, bool pPublishCard)
	: IfdMessage(IfdMessageType::IFDStatus)
	, mSlotName(pReaderInfo.getName())
	, mHasPinPad(!pReaderInfo.isBasicReader())
	, mMaxApduLength(pReaderInfo.getMaxApduLength())
	, mConnectedReader(pReaderInfo.isValid())
	, mCardAvailable(pReaderInfo.hasCard() && pPublishCard)
{
	if (mHasPinPad || Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		return;
	}

	switch (pReaderInfo.getPluginType())
	{
		case ReaderManagerPluginType::NFC:
		case ReaderManagerPluginType::SMART:
			mHasPinPad = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
			break;

		default:
			break;
	}
}


IfdStatus::IfdStatus(const QJsonObject& pMessageObject)
	: IfdMessage(pMessageObject)
	, mSlotName()
	, mHasPinPad(false)
	, mMaxApduLength(0)
	, mConnectedReader(false)
	, mCardAvailable(false)
{
	mSlotName = getStringValue(pMessageObject, SLOT_NAME());
	parsePinPad(pMessageObject);

	mMaxApduLength = getIntValue(pMessageObject, MAX_APDU_LENGTH(), -1);
	// Support SaC with AusweisApp < 1.22.0
	if (IfdVersion(IfdVersion::Version::v0).isSupported() && mMaxApduLength == 0)
	{
		mMaxApduLength = -1;
	}

	mConnectedReader = getBoolValue(pMessageObject, CONNECTED_READER());
	mCardAvailable = getBoolValue(pMessageObject, CARD_AVAILABLE());

	ensureType(IfdMessageType::IFDStatus);
}


const QString& IfdStatus::getSlotName() const
{
	return mSlotName;
}


bool IfdStatus::hasPinPad() const
{
	return mHasPinPad;
}


int IfdStatus::getMaxApduLength() const
{
	return mMaxApduLength;
}


bool IfdStatus::getConnectedReader() const
{
	return mConnectedReader;
}


bool IfdStatus::getCardAvailable() const
{
	return mCardAvailable;
}


QByteArray IfdStatus::toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_NAME()] = mSlotName;
	if (pIfdVersion >= IfdVersion::Version::v2)
	{
		result[PIN_PAD()] = mHasPinPad;
	}
	else
	{
		result[PIN_CAPABILITIES()] = createPaceCapabilities();
	}
	result[MAX_APDU_LENGTH()] = mMaxApduLength;
	result[CONNECTED_READER()] = mConnectedReader;
	result[CARD_AVAILABLE()] = mCardAvailable;
	result[EF_ATR()] = QJsonValue();
	result[EF_DIR()] = QJsonValue();

	return IfdMessage::toByteArray(result);
}
