/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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
		explicit IfdDisconnect(const QString& pReaderName);
		explicit IfdDisconnect(const QJsonObject& pMessageObject);
		~IfdDisconnect() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
