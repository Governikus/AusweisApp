/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"
#include "pinpad/EstablishPaceChannelOutput.h"


namespace governikus
{
class IfdEstablishPaceChannelResponse
	: public IfdMessageResponse
{
	private:
		QString mSlotHandle;
		EstablishPaceChannelOutput mOutputData;

		void parseOutputData(const QJsonObject& pMessageObject);

	public:
		IfdEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pOutputData, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdEstablishPaceChannelResponse(const QJsonObject& pMessageObject);
		~IfdEstablishPaceChannelResponse() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] const EstablishPaceChannelOutput& getOutputData() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
