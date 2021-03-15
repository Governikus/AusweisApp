/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EstablishPaceChannelOutput.h"
#include "RemoteMessageResponse.h"


namespace governikus
{
class IfdEstablishPaceChannelResponse
	: public RemoteMessageResponse
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
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
