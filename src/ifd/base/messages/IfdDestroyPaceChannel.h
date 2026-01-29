/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"
#include "IfdSlotHandle.h"


namespace governikus
{
class IfdDestroyPaceChannel
	: public IfdSlotHandle<IfdMessage>
{
	public:
		explicit IfdDestroyPaceChannel(const QString& pSlotHandle);
		explicit IfdDestroyPaceChannel(const QJsonObject& pMessageObject);
		~IfdDestroyPaceChannel() override = default;

		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
