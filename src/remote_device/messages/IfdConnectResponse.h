/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessageResponse.h"


namespace governikus
{
class IfdConnectResponse
	: public RemoteMessageResponse
{
	private:
		QString mSlotHandle;
		QString mError;

	public:
		IfdConnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		IfdConnectResponse(const QJsonObject& pMessageObject);
		virtual ~IfdConnectResponse() override = default;

		const QString& getSlotHandle() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
