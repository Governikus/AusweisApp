/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDestroyPaceChannel.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


IfdDestroyPaceChannel::IfdDestroyPaceChannel(const QString& pSlotHandle)
	: IfdSlotHandle<IfdMessage>(IfdMessageType::IFDDestroyPACEChannel, pSlotHandle)
{
}


IfdDestroyPaceChannel::IfdDestroyPaceChannel(const QJsonObject& pMessageObject)
	: IfdSlotHandle<IfdMessage>(pMessageObject)
{
	ensureType(IfdMessageType::IFDDestroyPACEChannel);
}


QByteArray IfdDestroyPaceChannel::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	return IfdMessage::toByteArray(result);
}
