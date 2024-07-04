/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"


namespace governikus
{
class IfdDestroyPaceChannelResponse
	: public IfdMessageResponse
{
	private:
		QString mSlotHandle;

	public:
		explicit IfdDestroyPaceChannelResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdDestroyPaceChannelResponse(const QJsonObject& pMessageObject);
		~IfdDestroyPaceChannelResponse() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
