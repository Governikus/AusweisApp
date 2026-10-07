/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once


#include "paos/element/Eac2InputType.h"
#include "paos/element/ElementParser.h"
#include "paos/retrieve/DidAuthenticateEac2.h"


namespace governikus
{

class DidAuthenticateEac2Parser
{
	private:
		const QSharedPointer<ElementParser> mParser;

		Eac2InputType parseEac2InputType();
		void parseCertificate(Eac2InputType& pEac2);
		void parseEphemeralPublicKey(Eac2InputType& pEac2, QString& pEphemeralPublicKey);
		void parseSignature(Eac2InputType& pEac2, QString& pSignature);

	public:
		explicit DidAuthenticateEac2Parser(const QSharedPointer<ElementParser>& pParser);

		[[nodiscard]] std::unique_ptr<DIDAuthenticateEAC2> parse();
};

} // namespace governikus
