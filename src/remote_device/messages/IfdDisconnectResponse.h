/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessageResponse.h"


namespace governikus
{
class IfdDisconnectResponse
	: public RemoteMessageResponse
{
	private:
		QString mSlotHandle;

	public:
		IfdDisconnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		IfdDisconnectResponse(const QJsonObject& pMessageObject);
		virtual ~IfdDisconnectResponse() override = default;

		const QString& getSlotHandle() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
