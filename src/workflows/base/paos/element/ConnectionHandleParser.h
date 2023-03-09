/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Parse an XML connection handle from given stream.
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
		explicit ConnectionHandleParser(QSharedPointer<QXmlStreamReader> pXmlReader);
		~ConnectionHandleParser() override;

		ConnectionHandle parse();
};

} // namespace governikus
