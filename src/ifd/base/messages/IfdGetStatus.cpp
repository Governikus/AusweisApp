/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */


#include "IfdGetStatus.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_NAME, "SlotName")
} // namespace


IfdGetStatus::IfdGetStatus(const QString& pSlotName)
	: IfdMessage(IfdMessageType::IFDGetStatus)
	, mSlotName(pSlotName)
{
}


IfdGetStatus::IfdGetStatus(const QJsonObject& pMessageObject)
	: IfdMessage(pMessageObject)
	, mSlotName()
{
	mSlotName = getStringValue(pMessageObject, SLOT_NAME());

	ensureType(IfdMessageType::IFDGetStatus);
}


const QString& IfdGetStatus::getSlotName() const
{
	return mSlotName;
}


QByteArray IfdGetStatus::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_NAME()] = mSlotName;

	return IfdMessage::toByteArray(result);
}
