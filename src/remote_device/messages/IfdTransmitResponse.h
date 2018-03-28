/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessageResponse.h"

#include <QByteArray>


namespace governikus
{
class IfdTransmitResponse
	: public RemoteMessageResponse
{
	private:
		QString mSlotHandle;
		QByteArray mResponseApdu;

	public:
		IfdTransmitResponse(const QString& pSlotHandle, const QByteArray& pResponseApdu = QByteArray(), const QString& pResultMinor = QString());
		IfdTransmitResponse(const QJsonObject& pMessageObject);
		virtual ~IfdTransmitResponse() override = default;

		const QString& getSlotHandle() const;
		const QByteArray& getResponseApdu() const;
		virtual QJsonDocument toJson(const QString& pContextHandle) const override;
};


} /* namespace governikus */
