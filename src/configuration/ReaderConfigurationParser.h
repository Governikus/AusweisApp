/*!
 * \brief Parser for reader configuration files
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderConfiguration.h"

#include <QJsonValue>
#include <QOperatingSystemVersion>

class test_ReaderConfigurationEntryParser;

namespace governikus
{
class ReaderConfigurationParser
{
	private:
		friend class ::test_ReaderConfigurationEntryParser;

		/**
		 * Parse a single entry of a reader configuration info list.
		 */
		class EntryParser
		{
			private:
				friend class ::test_ReaderConfigurationEntryParser;
				const QJsonValue mJsonValue;

				QString getDriverUrl(const QJsonObject& pObject) const;
				bool matchPlatform(const QJsonArray& pPlatforms, const QOperatingSystemVersion& pCurrentVersion = QOperatingSystemVersion::current()) const;
				ReaderConfigurationInfo fail(const QString& logMessage) const;

			public:
				EntryParser(const QJsonValue& pJsonValue);
				virtual ~EntryParser();

				ReaderConfigurationInfo parse() const;
		};

		static QVector<ReaderConfigurationInfo> fail(const QString& logMessage);
		static bool hasUniqueId(const ReaderConfigurationInfo& pInfo, const QVector<ReaderConfigurationInfo>& pInfos);

		ReaderConfigurationParser() = delete;
		~ReaderConfigurationParser() = delete;

	public:
		/*!
		 * Parses the configuration data and returns ReaderConfiguration.
		 * In case of any errors, the QSharedPointer is empty.
		 */
		static QVector<ReaderConfigurationInfo> parse(const QByteArray& pData);
};


} // namespace governikus
