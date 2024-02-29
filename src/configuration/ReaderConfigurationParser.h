/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Parser for reader configuration files
 */

#pragma once

#include "ReaderConfigurationInfo.h"

#include <QJsonValue>
#include <QOperatingSystemVersion>

class test_ReaderConfigurationEntryParser;

namespace governikus
{
class ReaderConfigurationParser
{
	friend class ::test_ReaderConfigurationEntryParser;

	private:
		/**
		 * Parse a single entry of a reader configuration info list.
		 */
		class EntryParser
		{
			friend class ::test_ReaderConfigurationEntryParser;

			private:
				const QJsonValue mJsonValue;

				[[nodiscard]] QString getDriverUrl(const QJsonObject& pObject) const;
				[[nodiscard]] bool matchPlatform(const QJsonArray& pPlatforms, const QOperatingSystemVersion& pCurrentVersion = QOperatingSystemVersion::current()) const;
				[[nodiscard]] ReaderConfigurationInfo fail(const QString& logMessage) const;
				[[nodiscard]] QSet<uint> getProductIds(const QJsonObject& object, bool* parseOk) const;

			public:
				explicit EntryParser(const QJsonValue& pJsonValue);
				virtual ~EntryParser();

				[[nodiscard]] ReaderConfigurationInfo parse() const;
		};

		static QList<ReaderConfigurationInfo> fail(const QString& logMessage);
		static bool hasUniqueId(const ReaderConfigurationInfo& pInfo, const QList<ReaderConfigurationInfo>& pInfos);

		ReaderConfigurationParser() = delete;
		~ReaderConfigurationParser() = delete;

	public:
		/*!
		 * Parses the configuration data and returns ReaderConfiguration.
		 * In case of any errors, the QSharedPointer is empty.
		 */
		static QList<ReaderConfigurationInfo> parse(const QByteArray& pData);
};


} // namespace governikus
