/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"


namespace governikus
{
class IfdGetStatus
	: public IfdMessage
{
	private:
		QString mSlotName;

	public:
		explicit IfdGetStatus(const QString& pSlotName = QString());
		explicit IfdGetStatus(const QJsonObject& pMessageObject);
		~IfdGetStatus() override = default;

		[[nodiscard]] const QString& getSlotName() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
