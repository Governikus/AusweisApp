/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDisconnectResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
} // namespace


IfdDisconnectResponse::IfdDisconnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: IfdMessageResponse(IfdMessageType::IFDDisconnectResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
{
}


IfdDisconnectResponse::IfdDisconnectResponse(const QJsonObject& pMessageObject)
	: IfdMessageResponse(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	if (getType() != IfdMessageType::IFDDisconnectResponse)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDDisconnectResponse"));
	}
}


const QString& IfdDisconnectResponse::getSlotHandle() const
{
	return mSlotHandle;
}


QByteArray IfdDisconnectResponse::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return IfdMessage::toByteArray(result);
}
