/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"
#include "IfdSlotHandle.h"


namespace governikus
{
class IfdConnectResponse
	: public IfdSlotHandle<IfdMessageResponse>
{
	private:
		QString mError;

	public:
		explicit IfdConnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdConnectResponse(const QJsonObject& pMessageObject);
		~IfdConnectResponse() override = default;

		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
