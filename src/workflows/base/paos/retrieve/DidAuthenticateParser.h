/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/element/ConnectionHandle.h"
#include "paos/element/ElementParser.h"
#include "paos/retrieve/DidAuthenticateMessage.h"

#include <QSharedPointer>

namespace governikus
{
class DidAuthenticateParser
{
	private:
		ConnectionHandle mConnectionHandle;
		QString mDidName;
		const QSharedPointer<ElementParser> mParser;

		[[nodiscard]] QStringView getElementType() const;

	public:
		explicit DidAuthenticateParser(const QSharedPointer<ElementParser>& pParser);
		[[nodiscard]] std::unique_ptr<DidAuthenticateMessage> parse();
};

} //namespace governikus
