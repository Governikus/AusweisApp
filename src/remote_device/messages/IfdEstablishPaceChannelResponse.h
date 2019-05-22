/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessageResponse.h"


namespace governikus
{
class IfdEstablishPaceChannelResponse
	: public RemoteMessageResponse
{
	private:
		QString mSlotHandle;
		QByteArray mOutputData;

	public:
		IfdEstablishPaceChannelResponse(const QString& pSlotHandle, const QByteArray& pOutputData, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		IfdEstablishPaceChannelResponse(const QJsonObject& pMessageObject);
		virtual ~IfdEstablishPaceChannelResponse() override = default;

		const QString& getSlotHandle() const;
		const QByteArray& getOutputData() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
