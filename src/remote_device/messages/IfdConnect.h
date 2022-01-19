/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"


namespace governikus
{
class IfdConnect
	: public RemoteMessage
{
	private:
		QString mSlotName;
		bool mExclusive;

	public:
		IfdConnect(const QString& pSlotName, bool pExclusive = true);
		explicit IfdConnect(const QJsonObject& pMessageObject);
		~IfdConnect() override = default;

		[[nodiscard]] const QString& getSlotName() const;
		[[nodiscard]] bool isExclusive() const;
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
