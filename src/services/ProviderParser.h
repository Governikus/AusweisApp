/*!
 * ProviderParser.h
 *
 * \brief Parser for provider configuration files
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once


#include "ProviderSettings.h"
#include "SettingsParser.h"

namespace governikus
{
class ProviderParser
	: public SettingsParser<ProviderSettings>
{
	public:
		ProviderParser();

		~ProviderParser();

		/*!
		 * Parses the configuration data and returns ProviderSettings.
		 * In case of any errors, the QSharedPointer is empty.
		 */
		virtual QSharedPointer<ProviderSettings> parse(const QByteArray& pData) override;
};


} /* namespace governikus */
