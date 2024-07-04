/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */


#include "IfdError.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
} // namespace


IfdError::IfdError(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: IfdMessageResponse(IfdMessageType::IFDError, pResultMinor)
	, mSlotHandle(pSlotHandle)
{
}


IfdError::IfdError(const QJsonObject& pMessageObject)
	: IfdMessageResponse(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	ensureType(IfdMessageType::IFDError);
}


const QString& IfdError::getSlotHandle() const
{
	return mSlotHandle;
}


QByteArray IfdError::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return IfdMessage::toByteArray(result);
}
