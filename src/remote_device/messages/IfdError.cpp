/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */


#include "IfdError.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
}


IfdError::IfdError(const QString& pSlotHandle, const QString& pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDError, pResultMinor)
	, mSlotHandle(pSlotHandle)
{
}


IfdError::IfdError(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());
}


const QString& IfdError::getSlotHandle() const
{
	return mSlotHandle;
}


QJsonDocument IfdError::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return QJsonDocument(result);
}
