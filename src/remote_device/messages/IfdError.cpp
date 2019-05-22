/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */


#include "IfdError.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
} // namespace


IfdError::IfdError(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDError, pResultMinor)
	, mSlotHandle(pSlotHandle)
{
}


IfdError::IfdError(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	if (getType() != RemoteCardMessageType::IFDError)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDError"));
	}
}


const QString& IfdError::getSlotHandle() const
{
	return mSlotHandle;
}


QByteArray IfdError::toByteArray(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return RemoteMessage::toByteArray(result);
}
