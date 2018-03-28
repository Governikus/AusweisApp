/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDisconnectResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
}


IfdDisconnectResponse::IfdDisconnectResponse(const QString& pSlotHandle, const QString& pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDDisconnectResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
{
}


IfdDisconnectResponse::IfdDisconnectResponse(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());
}


const QString& IfdDisconnectResponse::getSlotHandle() const
{
	return mSlotHandle;
}


QJsonDocument IfdDisconnectResponse::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return QJsonDocument(result);
}
