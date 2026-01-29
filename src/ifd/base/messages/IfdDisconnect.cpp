/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */


#include "IfdDisconnect.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


IfdDisconnect::IfdDisconnect(const QString& pSlotHandle)
	: IfdSlotHandle<IfdMessage>(IfdMessageType::IFDDisconnect, pSlotHandle)
{
}


IfdDisconnect::IfdDisconnect(const QJsonObject& pMessageObject)
	: IfdSlotHandle<IfdMessage>(pMessageObject)
{
	ensureType(IfdMessageType::IFDDisconnect);
}


QByteArray IfdDisconnect::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	return IfdMessage::toByteArray(result);
}
