/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/element/ElementParser.h"
#include "paos/retrieve/Transmit.h"


namespace governikus
{

class TransmitParser
{
	private:
		const QSharedPointer<ElementParser>& mParser;
		void parseInputApduInfo(Transmit& pTransmit);

	public:
		explicit TransmitParser(const QSharedPointer<ElementParser>& pParser);
		~TransmitParser();

		[[nodiscard]] std::unique_ptr<Transmit> parse();
};

} // namespace governikus
