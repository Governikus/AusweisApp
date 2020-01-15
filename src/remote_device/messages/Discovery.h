/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
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

	public:
		Discovery(const QString& pIfdName, const QString& pIfdId, quint16 pPort, const QVector<IfdVersion::Version>& pSupportedApis);
		Discovery(const QJsonObject& pMessageObject);
		virtual ~Discovery() override;

		const QString& getIfdName() const;
		const QString& getIfdId() const;
		quint16 getPort() const;
		const QVector<IfdVersion::Version>& getSupportedApis() const;

		virtual QByteArray toByteArray(const QString& pContextHandle = QString()) const override;
};


} // namespace governikus
