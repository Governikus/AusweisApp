/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"


namespace governikus
{
class IfdConnect
	: public IfdMessage
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
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
