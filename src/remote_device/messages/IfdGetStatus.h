/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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
		explicit IfdGetStatus(const QString& pSlotName = QString());
		explicit IfdGetStatus(const QJsonObject& pMessageObject);
		~IfdGetStatus() override = default;

		[[nodiscard]] const QString& getSlotName() const;
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
