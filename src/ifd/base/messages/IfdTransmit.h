/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"
#include "IfdSlotHandle.h"


namespace governikus
{
class IfdTransmit
	: public IfdSlotHandle<IfdMessage>
{
	private:
		QByteArray mInputApdu;
		QString mDisplayText;

		void parseInputApdu(const QJsonObject& pMessageObject);

	public:
		IfdTransmit(const QString& pSlotHandle, const QByteArray& pInputApdu, const QString& pDisplayText = QString());
		explicit IfdTransmit(const QJsonObject& pMessageObject);
		~IfdTransmit() override = default;

		[[nodiscard]] const QByteArray& getInputApdu() const;
		[[nodiscard]] const QString& getDisplayText() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
