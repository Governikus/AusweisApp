/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/PaosMessage.h"
#include "paos/element/ElementParser.h"

class test_PaosParser;

namespace governikus
{

class PaosParser
{
	friend class ::test_PaosParser;

	private:
		QSharedPointer<ElementParser> mParser;
		QString mMessageID;
		QString mRelatesTo;

		[[nodiscard]] std::unique_ptr<PaosMessage> parseMessage();
		[[nodiscard]] QSharedPointer<ElementParser> getParser();
		[[nodiscard]] std::unique_ptr<PaosMessage> parseEnvelope();
		[[nodiscard]] std::unique_ptr<PaosMessage> parseBody();
		void parseHeader();

	public:
		explicit PaosParser();
		virtual ~PaosParser();

		[[nodiscard]] std::unique_ptr<PaosMessage> parse(const QSharedPointer<ElementParser>& pParser);


};

} // namespace governikus
