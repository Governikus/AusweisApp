/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "ElementParser.h"

#include <QSharedPointer>
#include <QXmlStreamReader>


namespace governikus
{

class ConnectionHandle;

class ConnectionHandleParser
{
	public:
		explicit ConnectionHandleParser(const QSharedPointer<ElementParser>& pParser);

	private:
		QSharedPointer<ElementParser> mParser;

		void parseUniqueElementText(const std::function<void(const QString&)>& pFunc, QString& pText);

	public:
		ConnectionHandle parse();
};

} // namespace governikus
