/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"


namespace governikus
{
class IfdGetStatus
	: public RemoteMessage
{
	private:
		QString mSlotName;

	public:
		IfdGetStatus(const QString& pSlotName = QString());
		IfdGetStatus(const QJsonObject& pMessageObject);
		virtual ~IfdGetStatus() override = default;

		const QString& getSlotName() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
