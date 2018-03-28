/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDisconnect.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
}


IfdDisconnect::IfdDisconnect(const QString& pSlotHandle)
	: RemoteMessage(RemoteCardMessageType::IFDDisconnect)
	, mSlotHandle(pSlotHandle)
{
}


IfdDisconnect::IfdDisconnect(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());
}


const QString& IfdDisconnect::getSlotHandle() const
{
	return mSlotHandle;
}


QJsonDocument IfdDisconnect::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return QJsonDocument(result);
}
