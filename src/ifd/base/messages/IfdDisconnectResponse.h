/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"


namespace governikus
{
class IfdDisconnectResponse
	: public IfdMessageResponse
{
	private:
		QString mSlotHandle;

	public:
		IfdDisconnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdDisconnectResponse(const QJsonObject& pMessageObject);
		~IfdDisconnectResponse() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
