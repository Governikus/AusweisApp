/*!
 * \brief Parse an XML connection handle from given stream.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ElementParser.h"

#include <QSharedPointer>
#include <QXmlStreamReader>


namespace governikus
{

class ConnectionHandle;

class ConnectionHandleParser
	: public ElementParser
{
	public:
		ConnectionHandleParser(QSharedPointer<QXmlStreamReader> pXmlReader);
		virtual ~ConnectionHandleParser();

		ConnectionHandle parse();
};

} /* namespace governikus */
