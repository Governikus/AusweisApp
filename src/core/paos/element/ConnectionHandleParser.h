/*!
 * \brief Parse an XML connection handle from given stream.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "ConnectionHandle.h"
#include "ElementParser.h"

#include <QSharedPointer>
#include <QXmlStreamReader>


namespace governikus
{

class ConnectionHandleParser
	: public ElementParser
{
	public:
		ConnectionHandleParser(QSharedPointer<QXmlStreamReader> pXmlReader);
		virtual ~ConnectionHandleParser();

		ConnectionHandle parse();
};

} /* namespace governikus */
