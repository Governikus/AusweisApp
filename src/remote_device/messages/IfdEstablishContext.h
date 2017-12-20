/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DeviceInfo.h"
#include "RemoteMessage.h"

#include <QJsonObject>
#include <QString>


namespace governikus
{
class IfdEstablishContext
	: public RemoteMessage
{
	private:
		QString mProtocol;
		QString mUdName;

	public:
		IfdEstablishContext(const QString& pProtocol, const QString& pUdName);
		IfdEstablishContext(const QJsonObject& pMessageObject);
		virtual ~IfdEstablishContext() override = default;

		const QString& getProtocol() const;
		const QString& getUdName() const;
		virtual QJsonDocument toJson(const QString& pContextHandle) const override;
};


} /* namespace governikus */
