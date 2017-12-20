/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"

#include <QVector>


namespace governikus
{
class Discovery
{
	private:
		const QString mIfdName;
		const QString mIfdId;
		const quint16 mPort;
		const QStringList mSupportedApis;

	public:
		Discovery(const QString& pIfdName, const QString& pIfdId, quint16 pPort, const QStringList& pSupportedApis);
		~Discovery() = default;

		const QString& getIfdName() const;
		const QString& getIfdId() const;
		quint16 getPort() const;
		const QStringList& getSupportedApis() const;
		QJsonDocument toJson() const;
};


} /* namespace governikus */
