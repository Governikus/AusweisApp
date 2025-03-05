/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDisconnect.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
} // namespace


IfdDisconnect::IfdDisconnect(const QString& pSlotHandle)
	: IfdMessage(IfdMessageType::IFDDisconnect)
	, mSlotHandle(pSlotHandle)
{
}


IfdDisconnect::IfdDisconnect(const QJsonObject& pMessageObject)
	: IfdMessage(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	ensureType(IfdMessageType::IFDDisconnect);
}


const QString& IfdDisconnect::getSlotHandle() const
{
	return mSlotHandle;
}


QByteArray IfdDisconnect::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return IfdMessage::toByteArray(result);
}
