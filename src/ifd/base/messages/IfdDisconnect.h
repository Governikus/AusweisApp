/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"


namespace governikus
{
class IfdDisconnect
	: public IfdMessage
{
	private:
		QString mSlotHandle;

	public:
		explicit IfdDisconnect(const QString& pReaderName);
		explicit IfdDisconnect(const QJsonObject& pMessageObject);
		~IfdDisconnect() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
