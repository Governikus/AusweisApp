/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDisconnect.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
} // namespace


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

	if (getType() != RemoteCardMessageType::IFDDisconnect)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDDisconnect"));
	}
}


const QString& IfdDisconnect::getSlotHandle() const
{
	return mSlotHandle;
}


QByteArray IfdDisconnect::toByteArray(const IfdVersion&, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return RemoteMessage::toByteArray(result);
}
