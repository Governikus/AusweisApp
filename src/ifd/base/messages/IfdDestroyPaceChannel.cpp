/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDestroyPaceChannel.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
} // namespace


IfdDestroyPaceChannel::IfdDestroyPaceChannel(const QString& pSlotHandle)
	: IfdMessage(IfdMessageType::IFDDestroyPACEChannel)
	, mSlotHandle(pSlotHandle)
{
}


IfdDestroyPaceChannel::IfdDestroyPaceChannel(const QJsonObject& pMessageObject)
	: IfdMessage(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	ensureType(IfdMessageType::IFDDestroyPACEChannel);
}


const QString& IfdDestroyPaceChannel::getSlotHandle() const
{
	return mSlotHandle;
}


QByteArray IfdDestroyPaceChannel::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return IfdMessage::toByteArray(result);
}
