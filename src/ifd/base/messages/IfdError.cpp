/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */


#include "IfdError.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


IfdError::IfdError(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
	: IfdSlotHandle<IfdMessageResponse>(IfdMessageType::IFDError, pSlotHandle, pResultMinor)
{
}


IfdError::IfdError(const QJsonObject& pMessageObject)
	: IfdSlotHandle<IfdMessageResponse>(pMessageObject)
{
	ensureType(IfdMessageType::IFDError);
}


QByteArray IfdError::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	return IfdMessage::toByteArray(result);
}
