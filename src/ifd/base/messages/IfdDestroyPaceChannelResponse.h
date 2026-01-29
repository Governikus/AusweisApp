/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"
#include "IfdSlotHandle.h"


namespace governikus
{
class IfdDestroyPaceChannelResponse
	: public IfdSlotHandle<IfdMessageResponse>
{
	public:
		explicit IfdDestroyPaceChannelResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdDestroyPaceChannelResponse(const QJsonObject& pMessageObject);
		~IfdDestroyPaceChannelResponse() override = default;

		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
