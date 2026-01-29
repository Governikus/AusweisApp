/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDestroyPaceChannelResponse.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


IfdDestroyPaceChannelResponse::IfdDestroyPaceChannelResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: IfdSlotHandle<IfdMessageResponse>(IfdMessageType::IFDDestroyPACEChannelResponse, pSlotHandle, pResultMinor)
{
}


IfdDestroyPaceChannelResponse::IfdDestroyPaceChannelResponse(const QJsonObject& pMessageObject)
	: IfdSlotHandle<IfdMessageResponse>(pMessageObject)
{
	ensureType(IfdMessageType::IFDDestroyPACEChannelResponse);
}


QByteArray IfdDestroyPaceChannelResponse::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	return IfdMessage::toByteArray(result);
}
