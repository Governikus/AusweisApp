/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"

#include <QByteArray>


namespace governikus
{
class IfdTransmit
	: public RemoteMessage
{
	private:
		QString mSlotHandle;
		QByteArray mInputApdu;

		void parseCommandApdu(const QJsonValue& pEntry);

	public:
		IfdTransmit(const QString& pSlotHandle, const QByteArray& pInputApdu);
		IfdTransmit(const QJsonObject& pMessageObject);
		virtual ~IfdTransmit() override = default;

		const QString& getSlotHandle() const;
		const QByteArray& getInputApdu() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
