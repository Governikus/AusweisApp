/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDisconnectResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
} // namespace


IfdDisconnectResponse::IfdDisconnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDDisconnectResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
{
}


IfdDisconnectResponse::IfdDisconnectResponse(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	if (getType() != RemoteCardMessageType::IFDDisconnectResponse)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDDisconnectResponse"));
	}
}


const QString& IfdDisconnectResponse::getSlotHandle() const
{
	return mSlotHandle;
}


QByteArray IfdDisconnectResponse::toByteArray(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return RemoteMessage::toByteArray(result);
}
