/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "GetIfdStatus.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_NAME, "SlotName")
}


GetIfdStatus::GetIfdStatus(const QString& pSlotName)
	: RemoteMessage(RemoteCardMessageType::IFDGetStatus)
	, mSlotName(pSlotName)
{
}


GetIfdStatus::GetIfdStatus(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mSlotName()
{
	mSlotName = getStringValue(pMessageObject, SLOT_NAME());
}


const QString& GetIfdStatus::getSlotName() const
{
	return mSlotName;
}


QJsonDocument GetIfdStatus::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_NAME()] = mSlotName;

	return QJsonDocument(result);
}
