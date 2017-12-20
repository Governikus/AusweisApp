/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"


namespace governikus
{
class IfdDisconnect
	: public RemoteMessage
{
	private:
		QString mSlotHandle;

	public:
		IfdDisconnect(const QString& pReaderName);
		IfdDisconnect(const QJsonObject& pMessageObject);
		virtual ~IfdDisconnect() override = default;

		const QString& getSlotHandle() const;
		virtual QJsonDocument toJson(const QString& pContextHandle) const override;
};


} /* namespace governikus */
