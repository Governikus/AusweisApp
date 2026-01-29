/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardReturnCode.h"
#include "IfdMessageResponse.h"
#include "IfdSlotHandle.h"


namespace governikus
{
class IfdModifyPinResponse
	: public IfdSlotHandle<IfdMessageResponse>
{
	private:
		QByteArray mOutputData;

	public:
		IfdModifyPinResponse(const QString& pSlotHandle, const QByteArray& pOutputData, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdModifyPinResponse(const QJsonObject& pMessageObject);
		~IfdModifyPinResponse() override = default;

		[[nodiscard]] const QByteArray& getOutputData() const;
		[[nodiscard]] CardReturnCode getReturnCode() const;

		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
