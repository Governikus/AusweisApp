/*!
 * \brief Parser for provider configuration files
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CallCost.h"
#include "ProviderConfigurationInfo.h"

#include <QByteArray>
#include <QJsonArray>
#include <QString>

class test_ProviderConfigurationParser;

namespace governikus
{
class ProviderConfigurationParser
{
	private:
		friend class ::test_ProviderConfigurationParser;
		static bool isExcludedPlatform(const QJsonArray& pExcludedArray, QLatin1String pCurrentOS);
		static QVector<ProviderConfigurationInfo> parseProvider(const QByteArray& pData, QLatin1String pCurrentOS);

		ProviderConfigurationParser() = delete;
		~ProviderConfigurationParser() = delete;

	public:
		static QMap<QString, CallCost> parseCallCosts(const QByteArray& pData);
		static QVector<ProviderConfigurationInfo> parseProvider(const QByteArray& pData);
};


} /* namespace governikus */
