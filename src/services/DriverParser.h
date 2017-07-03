/*!
 * DriverParser.h
 *
 * \brief Parser for driver configuration files
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once


#include "DriverSettings.h"
#include "SettingsParser.h"


namespace governikus
{
class DriverParser
	: public SettingsParser<DriverSettings>
{
	private:
		QSharedPointer<DriverSettings> fail(const QString& logMessage);

	public:
		DriverParser();

		virtual ~DriverParser();

		/*!
		 * Parses the configuration data and returns DriverSettings.
		 * In case of any errors, the QSharedPointer is empty.
		 */
		virtual QSharedPointer<DriverSettings> parse(const QByteArray& pData) override;
};


} /* namespace governikus */
