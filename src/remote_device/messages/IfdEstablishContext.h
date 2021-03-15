/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
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
		~IfdEstablishContext() override = default;

		[[nodiscard]] const IfdVersion& getProtocol() const;
		[[nodiscard]] const QString& getProtocolRaw() const;
		[[nodiscard]] const QString& getUdName() const;
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
