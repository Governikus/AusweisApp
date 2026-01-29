/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDisconnectResponse.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


IfdDisconnectResponse::IfdDisconnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: IfdSlotHandle<IfdMessageResponse>(IfdMessageType::IFDDisconnectResponse, pSlotHandle, pResultMinor)
{
}


IfdDisconnectResponse::IfdDisconnectResponse(const QJsonObject& pMessageObject)
	: IfdSlotHandle<IfdMessageResponse>(pMessageObject)
{
	ensureType(IfdMessageType::IFDDisconnectResponse);
}


QByteArray IfdDisconnectResponse::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	return IfdMessage::toByteArray(result);
}
