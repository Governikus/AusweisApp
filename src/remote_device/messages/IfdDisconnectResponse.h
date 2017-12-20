/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
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
		IfdDisconnectResponse(const QString& pSlotHandle, const QString& pResultMinor = QString());
		IfdDisconnectResponse(const QJsonObject& pMessageObject);
		virtual ~IfdDisconnectResponse() override = default;

		const QString& getSlotHandle() const;
		virtual QJsonDocument toJson(const QString& pContextHandle) const override;
};


} /* namespace governikus */
