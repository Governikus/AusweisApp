/*!
 * \brief Classes that model the GetIFDStatus message.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"


namespace governikus
{
class GetIfdStatus
	: public RemoteMessage
{
	private:
		QString mSlotName;

	public:
		GetIfdStatus(const QString& pSlotName = QString());
		GetIfdStatus(const QJsonObject& pMessageObject);
		virtual ~GetIfdStatus() override = default;

		const QString& getSlotName() const;
		virtual QJsonDocument toJson(const QString& pContextHandle) const override;
};


} /* namespace governikus */
