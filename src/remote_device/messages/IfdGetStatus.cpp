/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */


#include "IfdGetStatus.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_NAME, "SlotName")
} // namespace


IfdGetStatus::IfdGetStatus(const QString& pSlotName)
	: RemoteMessage(RemoteCardMessageType::IFDGetStatus)
	, mSlotName(pSlotName)
{
}


IfdGetStatus::IfdGetStatus(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mSlotName()
{
	mSlotName = getStringValue(pMessageObject, SLOT_NAME());

	if (getType() != RemoteCardMessageType::IFDGetStatus)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDGetStatus"));
	}
}


const QString& IfdGetStatus::getSlotName() const
{
	return mSlotName;
}


QByteArray IfdGetStatus::toByteArray(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_NAME()] = mSlotName;

	return RemoteMessage::toByteArray(result);
}
