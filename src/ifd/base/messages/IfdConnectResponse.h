/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"


namespace governikus
{
class IfdConnectResponse
	: public IfdMessageResponse
{
	private:
		QString mSlotHandle;
		QString mError;

	public:
		IfdConnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdConnectResponse(const QJsonObject& pMessageObject);
		~IfdConnectResponse() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
