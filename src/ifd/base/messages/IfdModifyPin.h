/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"


namespace governikus
{

class IfdModifyPin
	: public IfdMessage
{
	private:
		QString mSlotHandle;
		QByteArray mInputData;

	public:
		IfdModifyPin(const QString& pSlotHandle = QString(), const QByteArray& pInputData = QByteArray());
		explicit IfdModifyPin(const QJsonObject& pMessageObject);
		~IfdModifyPin() override = default;

		[[nodiscard]] bool isValid() const;
		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] const QByteArray& getInputData() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
