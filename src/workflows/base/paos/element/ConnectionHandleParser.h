/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
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

	private:
		void parseUniqueElementText(const std::function<void(const QString&)>& pFunc, QString& pText);

	public:
		ConnectionHandle parse();
};

} // namespace governikus
