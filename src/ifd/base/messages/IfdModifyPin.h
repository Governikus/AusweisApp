/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"
#include "IfdSlotHandle.h"


namespace governikus
{

class IfdModifyPin
	: public IfdSlotHandle<IfdMessage>
{
	private:
		QByteArray mInputData;

	public:
		explicit IfdModifyPin(const QString& pSlotHandle = QString(), const QByteArray& pInputData = QByteArray());
		explicit IfdModifyPin(const QJsonObject& pMessageObject);
		~IfdModifyPin() override = default;

		[[nodiscard]] bool isValid() const;
		[[nodiscard]] const QByteArray& getInputData() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
