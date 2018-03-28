/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdVersion.h"
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
		const QVector<IfdVersion::Version> mSupportedApis;

	public:
		Discovery(const QString& pIfdName, const QString& pIfdId, quint16 pPort, const QVector<IfdVersion::Version>& pSupportedApis);
		~Discovery() = default;

		const QString& getIfdName() const;
		const QString& getIfdId() const;
		quint16 getPort() const;
		const QVector<IfdVersion::Version>& getSupportedApis() const;
		QJsonDocument toJson() const;
};


} /* namespace governikus */
