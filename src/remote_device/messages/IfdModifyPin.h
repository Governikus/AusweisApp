/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"


namespace governikus
{

class IfdModifyPin
	: public RemoteMessage
{
	private:
		QString mSlotHandle;
		QByteArray mInputData;

	public:
		IfdModifyPin(const QString& pSlotHandle = QString(), const QByteArray& pInputData = QByteArray());
		IfdModifyPin(const QJsonObject& pMessageObject);
		virtual ~IfdModifyPin() override = default;

		const QString& getSlotHandle() const;
		const QByteArray& getInputData() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
