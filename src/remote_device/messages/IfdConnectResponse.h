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
		IfdConnectResponse(const QString& pSlotHandle, const QString& pResultMinor = QString());
		IfdConnectResponse(const QJsonObject& pMessageObject);
		virtual ~IfdConnectResponse() override = default;

		const QString& getSlotHandle() const;
		virtual QJsonDocument toJson(const QString& pContextHandle) const override;
};


} /* namespace governikus */
