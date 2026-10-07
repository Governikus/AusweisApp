/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "IfdMessageResponse.h"
#include "IfdSlotHandle.h"
#include "pinpad/EstablishPaceChannelOutput.h"


namespace governikus
{
class IfdEstablishPaceChannelResponse
	: public IfdSlotHandle<IfdMessageResponse>
{
	private:
		EstablishPaceChannelOutput mOutputData;

		void parseOutputData(const QJsonObject& pMessageObject);

	public:
		IfdEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pOutputData, ECardApiResult::Minor pResultMinor);
		explicit IfdEstablishPaceChannelResponse(PacePasswordId pPasswordId, const QJsonObject& pMessageObject);
		~IfdEstablishPaceChannelResponse() override = default;

		[[nodiscard]] const EstablishPaceChannelOutput& getOutputData() const;
		[[nodiscard]] CardReturnCode getReturnCode() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
