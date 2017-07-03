/*!
 * SupportedReaders.h
 *
 * \brief Implements whitelist of readers and provides some more useful information of them.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "SmartCardDefinitions.h"

#include <QMap>
#include <QRegularExpression>
#include <QString>

namespace governikus
{


class SupportedReaders
{
	private:
		typedef QMap<ReaderType, QRegularExpression> WhiteListMap;
		const WhiteListMap mWhiteList;

		Q_DISABLE_COPY(SupportedReaders)

		WhiteListMap::const_iterator getMapEntry(const QString& pReaderName) const;

	protected:
		SupportedReaders();
		~SupportedReaders();

	public:
		static SupportedReaders& getInstance();

		/*!
		 * Returns the corresponding \ref Reader object of given reader name. If reader name is unknown it will return \ref Reader::UNKNOWN.
		 */
		ReaderType getReader(const QString& pReaderName) const;

		/*!
		 * Check if the given reader name is on whitelist
		 */
		bool isOnWhiteList(const QString& pReaderName) const;

		/*!
		 * Returns the base name of given \ref Reader, otherwise QString()
		 */
		QString getPattern(ReaderType pReader) const;
};

} /* namespace governikus */
