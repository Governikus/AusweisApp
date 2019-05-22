/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */


#include "IfdStatus.h"

#include "AppSettings.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_NAME, "SlotName")
VALUE_NAME(PIN_CAPABILITIES, "PINCapabilities")
VALUE_NAME(PIN_CAP_PACE, "PACE")
VALUE_NAME(PIN_CAP_EID, "eID")
VALUE_NAME(PIN_CAP_ESIGN, "eSign")
VALUE_NAME(PIN_CAP_DESTROY, "Destroy")
VALUE_NAME(MAX_APDU_LENGTH, "MaxAPDULength")
VALUE_NAME(CONNECTED_READER, "ConnectedReader")
VALUE_NAME(CARD_AVAILABLE, "CardAvailable")
VALUE_NAME(EF_ATR, "EFATR")
VALUE_NAME(EF_DIR, "EFDIR")
} // namespace


PaceCapabilities::PaceCapabilities(bool pPace, bool pEId, bool pESign, bool pDestroy)
	: mPace(pPace)
	, mEId(pEId)
	, mESign(pESign)
	, mDestroy(pDestroy)
{
}


bool PaceCapabilities::getPace() const
{
	return mPace;
}


bool PaceCapabilities::getEId() const
{
	return mEId;
}


bool PaceCapabilities::getESign() const
{
	return mESign;
}


bool PaceCapabilities::getDestroy() const
{
	return mDestroy;
}


QJsonValue PaceCapabilities::toJson() const
{
	QJsonObject result;
	result[PIN_CAP_PACE()] = mPace;
	result[PIN_CAP_EID()] = mEId;
	result[PIN_CAP_ESIGN()] = mESign;
	result[PIN_CAP_DESTROY()] = mDestroy;
	return result;
}


IfdStatus::IfdStatus(const QString& pSlotName,
		const PaceCapabilities& pPaceCapabilities,
		int pMaxApduLength,
		bool pConnected,
		bool pCardAvailable)
	: RemoteMessage(RemoteCardMessageType::IFDStatus)
	, mSlotName(pSlotName)
	, mPaceCapabilities(pPaceCapabilities)
	, mMaxApduLength(pMaxApduLength)
	, mConnectedReader(pConnected)
	, mCardAvailable(pCardAvailable)
{
}


IfdStatus::IfdStatus(const ReaderInfo& pReaderInfo)
	: IfdStatus(pReaderInfo.getName()
			, pReaderInfo.isBasicReader() ? Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode() : true
			, pReaderInfo.getMaxApduLength()
			, pReaderInfo.isConnected()
			, pReaderInfo.hasCard())
{
}


IfdStatus::IfdStatus(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mSlotName()
	, mPaceCapabilities()
	, mMaxApduLength(0)
	, mConnectedReader(false)
	, mCardAvailable(false)
{
	mSlotName = getStringValue(pMessageObject, SLOT_NAME());

	if (pMessageObject.contains(PIN_CAPABILITIES()))
	{
		QJsonValue value = pMessageObject.value(PIN_CAPABILITIES());
		if (value.isObject())
		{
			QJsonObject object = value.toObject();
			bool pace = getBoolValue(object, PIN_CAP_PACE());
			bool eId = getBoolValue(object, PIN_CAP_EID());
			bool eSign = getBoolValue(object, PIN_CAP_ESIGN());
			bool destroy = getBoolValue(object, PIN_CAP_DESTROY());
			mPaceCapabilities = PaceCapabilities(pace, eId, eSign, destroy);
		}
		else
		{
			invalidType(PIN_CAPABILITIES(), QLatin1String("object"));
		}
	}
	else
	{
		missingValue(PIN_CAPABILITIES());
	}

	mMaxApduLength = getIntValue(pMessageObject, MAX_APDU_LENGTH());
	mConnectedReader = getBoolValue(pMessageObject, CONNECTED_READER());
	mCardAvailable = getBoolValue(pMessageObject, CARD_AVAILABLE());

	if (getType() != RemoteCardMessageType::IFDStatus)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDStatus"));
	}
}


const QString& IfdStatus::getSlotName() const
{
	return mSlotName;
}


const PaceCapabilities& IfdStatus::getPaceCapabilities() const
{
	return mPaceCapabilities;
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


QByteArray IfdStatus::toByteArray(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_NAME()] = mSlotName;
	result[PIN_CAPABILITIES()] = mPaceCapabilities.toJson();
	result[MAX_APDU_LENGTH()] = mMaxApduLength;
	result[CONNECTED_READER()] = mConnectedReader;
	result[CARD_AVAILABLE()] = mCardAvailable;
	result[EF_ATR()] = QJsonValue();
	result[EF_DIR()] = QJsonValue();

	return RemoteMessage::toByteArray(result);
}
