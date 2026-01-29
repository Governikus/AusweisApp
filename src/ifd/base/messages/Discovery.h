/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"
#include "IfdVersion.h"

#include <QByteArray>
#include <QHostAddress>
#include <QList>
#include <QSet>
#include <QUrl>


namespace governikus
{
class Discovery
	: public IfdMessage
{
	private:
		QString mIfdName;
		QByteArray mIfdId;
		quint16 mPort;
		QSet<QUrl> mAddresses;
		QList<IfdVersion::Version> mSupportedApis;
		bool mPairing;

		void parseSupportedApi(const QJsonObject& pMessageObject);
		void parseIfdId(const QJsonObject& pMessageObject);
		void parseAddresses(const QJsonObject& pMessageObject);
		void parsePairing(const QJsonObject& pMessageObject);

	public:
		Discovery(const QString& pIfdName, const QByteArray& pIfdId, quint16 pPort, const QList<IfdVersion::Version>& pSupportedApis, bool pPairing = false);
		explicit Discovery(const QJsonObject& pMessageObject);
		~Discovery() override = default;

		[[nodiscard]] bool isSupported() const;
		[[nodiscard]] const QString& getIfdName() const;
		[[nodiscard]] const QByteArray& getIfdId() const;
		[[nodiscard]] quint16 getPort() const;
		[[nodiscard]] const QList<IfdVersion::Version>& getSupportedApis() const;

		void setPairing(bool pEnabled);
		[[nodiscard]] bool isPairing() const;

		void setAddresses(const QSet<QHostAddress>& pAddresses);
		[[nodiscard]] bool addressesMissing() const;
		[[nodiscard]] const QSet<QUrl>& getAddresses() const;
		[[nodiscard]] bool isLocalIfd() const;

		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle = QString()) const override;
};


} // namespace governikus
