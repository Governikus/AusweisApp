/*!
 * \brief Parser for reader configuration files
 *
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
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

				[[nodiscard]] QString getDriverUrl(const QJsonObject& pObject) const;
				[[nodiscard]] bool matchPlatform(const QJsonArray& pPlatforms, const QOperatingSystemVersion& pCurrentVersion = QOperatingSystemVersion::current()) const;
				[[nodiscard]] ReaderConfigurationInfo fail(const QString& logMessage) const;

			public:
				explicit EntryParser(const QJsonValue& pJsonValue);
				virtual ~EntryParser();

				[[nodiscard]] ReaderConfigurationInfo parse() const;
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
