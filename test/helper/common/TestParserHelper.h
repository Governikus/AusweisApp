/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/element/ElementParser.h"

#include <QByteArray>
#include <QSharedPointer>

namespace governikus
{

class TestParserHelper
{
	public:
		static QSharedPointer<ElementParser> create(const QByteArray& pContent);
		static QSharedPointer<ElementParser> create(const QString& pFile);
};

} // namespace governikus
