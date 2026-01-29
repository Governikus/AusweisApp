/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"
#include "IfdSlotHandle.h"


namespace governikus
{
class IfdError
	: public IfdSlotHandle<IfdMessageResponse>
{
	public:
		explicit IfdError(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdError(const QJsonObject& pMessageObject);
		~IfdError() override = default;

		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
