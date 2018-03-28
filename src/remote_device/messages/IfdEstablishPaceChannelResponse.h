/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EstablishPACEChannel.h"
#include "RemoteMessageResponse.h"
#include "SmartCardDefinitions.h"


namespace governikus
{
class IfdEstablishPaceChannelResponse
	: public RemoteMessageResponse
{
	private:
		QString mSlotHandle;
		QByteArray mOutputData;

	public:
		IfdEstablishPaceChannelResponse(const QString& pSlotHandle, const QByteArray& pOutputData, const QString& pResultMinor = QString());
		IfdEstablishPaceChannelResponse(const QJsonObject& pMessageObject);
		virtual ~IfdEstablishPaceChannelResponse() override = default;

		const QString& getSlotHandle() const;
		const QByteArray& getOutputData() const;
		virtual QJsonDocument toJson(const QString& pContextHandle) const override;
};


} /* namespace governikus */
