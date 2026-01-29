/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */


#include "IfdConnectResponse.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


IfdConnectResponse::IfdConnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: IfdSlotHandle<IfdMessageResponse>(IfdMessageType::IFDConnectResponse, pSlotHandle, pResultMinor)
{
}


IfdConnectResponse::IfdConnectResponse(const QJsonObject& pMessageObject)
	: IfdSlotHandle<IfdMessageResponse>(pMessageObject)
{
	mError = pMessageObject.value(QLatin1String("error")).toString();

	ensureType(IfdMessageType::IFDConnectResponse);
}


QByteArray IfdConnectResponse::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	return IfdMessage::toByteArray(result);
}
