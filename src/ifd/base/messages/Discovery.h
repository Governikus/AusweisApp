/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"
#include "IfdVersion.h"

#include <QByteArray>
#include <QList>


namespace governikus
{
class Discovery
	: public IfdMessage
{
	private:
		QString mIfdName;
		QByteArray mIfdId;
		quint16 mPort;
		QList<IfdVersion::Version> mSupportedApis;
		bool mPairing;

		void parseSupportedApi(const QJsonObject& pMessageObject);
		void parseIfdId(const QJsonObject& pMessageObject);
		void parsePairing(const QJsonObject& pMessageObject);

	public:
		Discovery(const QString& pIfdName, const QByteArray& pIfdId, quint16 pPort, const QList<IfdVersion::Version>& pSupportedApis, bool pPairing = false);
		explicit Discovery(const QJsonObject& pMessageObject);
		~Discovery() override = default;

		[[nodiscard]] const QString& getIfdName() const;
		[[nodiscard]] const QByteArray& getIfdId() const;
		[[nodiscard]] quint16 getPort() const;
		[[nodiscard]] const QList<IfdVersion::Version>& getSupportedApis() const;

		void setPairing(bool pEnabled);
		[[nodiscard]] bool getPairing() const;

		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle = QString()) const override;
};


} // namespace governikus
