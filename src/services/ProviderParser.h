/*!
 * \brief Parser for provider configuration files
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ProviderSettings.h"
#include "SettingsParser.h"

#include <QJsonArray>
#include <QString>

class test_ProviderParser;

namespace governikus
{
class ProviderParser
	: public SettingsParser<ProviderSettings>
{
	private:
		friend class ::test_ProviderParser;
		static bool isExcludedPlatform(const QJsonArray& pExcludedArray, QLatin1String pCurrentOS);
		QSharedPointer<ProviderSettings> parse(const QByteArray& pData, QLatin1String pCurrentOS);

	public:
		ProviderParser();
		virtual ~ProviderParser();

		/*!
		 * Parses the configuration data and returns ProviderSettings.
		 * In case of any errors, the QSharedPointer is empty.
		 */
		virtual QSharedPointer<ProviderSettings> parse(const QByteArray& pData) override;
};


} /* namespace governikus */
