/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDestroyPaceChannelResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
} // namespace


IfdDestroyPaceChannelResponse::IfdDestroyPaceChannelResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: IfdMessageResponse(IfdMessageType::IFDDestroyPACEChannelResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
{
}


IfdDestroyPaceChannelResponse::IfdDestroyPaceChannelResponse(const QJsonObject& pMessageObject)
	: IfdMessageResponse(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	ensureType(IfdMessageType::IFDDestroyPACEChannelResponse);
}


const QString& IfdDestroyPaceChannelResponse::getSlotHandle() const
{
	return mSlotHandle;
}


QByteArray IfdDestroyPaceChannelResponse::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return IfdMessage::toByteArray(result);
}
