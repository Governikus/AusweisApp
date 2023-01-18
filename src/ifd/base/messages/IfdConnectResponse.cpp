/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */


#include "IfdConnectResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
} // namespace


IfdConnectResponse::IfdConnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: IfdMessageResponse(IfdMessageType::IFDConnectResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
{
}


IfdConnectResponse::IfdConnectResponse(const QJsonObject& pMessageObject)
	: IfdMessageResponse(pMessageObject)
	, mSlotHandle()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());
	mError = pMessageObject.value(QLatin1String("error")).toString();

	if (getType() != IfdMessageType::IFDConnectResponse)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDConnectResponse"));
	}
}


const QString& IfdConnectResponse::getSlotHandle() const
{
	return mSlotHandle;
}


QByteArray IfdConnectResponse::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;

	return IfdMessage::toByteArray(result);
}
