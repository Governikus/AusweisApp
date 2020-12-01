/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DeviceInfo.h"
#include "IfdVersion.h"
#include "RemoteMessage.h"

#include <QJsonObject>
#include <QString>


namespace governikus
{
class IfdEstablishContext
	: public RemoteMessage
{
	private:
		QString mProtocolRaw;
		IfdVersion mProtocol;
		QString mUdName;

	public:
		IfdEstablishContext(const IfdVersion& pProtocol, const QString& pUdName);
		explicit IfdEstablishContext(const QJsonObject& pMessageObject);
		virtual ~IfdEstablishContext() override = default;

		const IfdVersion& getProtocol() const;
		const QString& getProtocolRaw() const;
		const QString& getUdName() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
