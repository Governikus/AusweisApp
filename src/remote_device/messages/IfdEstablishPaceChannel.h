/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MessageReceiver.h"
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
		IfdEstablishPaceChannel(const QJsonObject& pMessageObject);
		virtual ~IfdEstablishPaceChannel() override = default;

		const QString& getSlotHandle() const;
		const QByteArray& getInputData() const;
		virtual QJsonDocument toJson(const QString& pContextHandle) const override;
};


} /* namespace governikus */
