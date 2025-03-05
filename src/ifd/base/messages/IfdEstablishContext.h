/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DeviceInfo.h"
#include "IfdMessage.h"
#include "IfdVersion.h"

#include <QJsonObject>
#include <QString>


namespace governikus
{
class IfdEstablishContext
	: public IfdMessage
{
	private:
		QString mProtocolRaw;
		IfdVersion mProtocol;
		QString mUdName;

	public:
		IfdEstablishContext(const IfdVersion::Version& pVersion, const QString& pUdName);
		explicit IfdEstablishContext(const QJsonObject& pMessageObject);
		~IfdEstablishContext() override = default;

		[[nodiscard]] const IfdVersion& getProtocol() const;
		[[nodiscard]] const QString& getProtocolRaw() const;
		[[nodiscard]] const QString& getUdName() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
