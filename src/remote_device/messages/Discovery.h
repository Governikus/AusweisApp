/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdVersion.h"
#include "RemoteMessage.h"

#include <QVector>


namespace governikus
{
class Discovery
	: public RemoteMessage
{
	private:
		QString mIfdName;
		QString mIfdId;
		quint16 mPort;
		QVector<IfdVersion::Version> mSupportedApis;
		bool mPairing;

		void parseSupportedApi(const QJsonObject& pMessageObject);
		void parseIfdId(const QJsonObject& pMessageObject);
		void parsePairing(const QJsonObject& pMessageObject);

	public:
		Discovery(const QString& pIfdName, const QString& pIfdId, quint16 pPort, const QVector<IfdVersion::Version>& pSupportedApis);
		explicit Discovery(const QJsonObject& pMessageObject);
		~Discovery() override;

		[[nodiscard]] const QString& getIfdName() const;
		[[nodiscard]] const QString& getIfdId() const;
		[[nodiscard]] quint16 getPort() const;
		[[nodiscard]] const QVector<IfdVersion::Version>& getSupportedApis() const;

		void setPairing(bool pEnabled);
		[[nodiscard]] bool getPairing() const;

		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle = QString()) const override;
};


} // namespace governikus
