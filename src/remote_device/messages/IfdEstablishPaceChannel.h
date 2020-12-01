/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"


namespace governikus
{

class IfdEstablishPaceChannel
	: public RemoteMessage
{
	private:
		QString mSlotHandle;
		QByteArray mInputData;

	public:
		IfdEstablishPaceChannel(const QString& pSlotHandle = QString(), const QByteArray& pInputData = QByteArray());
		explicit IfdEstablishPaceChannel(const QJsonObject& pMessageObject);
		virtual ~IfdEstablishPaceChannel() override = default;

		const QString& getSlotHandle() const;
		const QByteArray& getInputData() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
