/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/element/ElementParser.h"
#include "paos/retrieve/DidAuthenticateEacAdditional.h"

#include <QString>


namespace governikus
{

class DidAuthenticateEacAdditionalParser
{
	private:
		const QSharedPointer<ElementParser> mParser;

		QString parseEacAdditionalInputType();

	public:
		explicit DidAuthenticateEacAdditionalParser(const QSharedPointer<ElementParser>& pParser);

		[[nodiscard]] std::unique_ptr<DIDAuthenticateEACAdditional> parse();
};

} // namespace governikus
