/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"
#include "IfdSlotHandle.h"


namespace governikus
{
class IfdDisconnect
	: public IfdSlotHandle<IfdMessage>
{
	public:
		explicit IfdDisconnect(const QString& pReaderName);
		explicit IfdDisconnect(const QJsonObject& pMessageObject);
		~IfdDisconnect() override = default;

		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
