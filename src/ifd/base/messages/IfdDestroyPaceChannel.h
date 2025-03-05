/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"


namespace governikus
{
class IfdDestroyPaceChannel
	: public IfdMessage
{
	private:
		QString mSlotHandle;

	public:
		explicit IfdDestroyPaceChannel(const QString& pSlotHandle);
		explicit IfdDestroyPaceChannel(const QJsonObject& pMessageObject);
		~IfdDestroyPaceChannel() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
