/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */


#include "IfdConnectResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
}


IfdConnectResponse::IfdConnectResponse(const QString& pSlotHandle, const QString& pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDConnectResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
{
}


IfdConnectResponse::IfdConnectResponse(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());
	mError = pMessageObject.value(QLatin1String("error")).toString();
}


const QString& IfdConnectResponse::getSlotHandle() const
{
	return mSlotHandle;
}


QJsonDocument IfdConnectResponse::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return QJsonDocument(result);
}
