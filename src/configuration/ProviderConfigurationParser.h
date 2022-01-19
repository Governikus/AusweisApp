/*!
 * \brief Parser for provider configuration files
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CallCost.h"
#include "ProviderConfigurationInfo.h"

#include <QByteArray>
#include <QJsonArray>
#include <QOperatingSystemVersion>
#include <QString>


namespace governikus
{
class ProviderConfigurationParser
{
	private:
		friend class ::test_ProviderConfigurationParser;
		static QVector<ProviderConfigurationInfo> parseProvider(const QByteArray& pData, const QOperatingSystemVersion& pCurrentOS);

		ProviderConfigurationParser() = delete;
		~ProviderConfigurationParser() = delete;

	public:
		static QMap<QString, CallCost> parseCallCosts(const QByteArray& pData);
		static QVector<ProviderConfigurationInfo> parseProvider(const QByteArray& pData);
};


} // namespace governikus
