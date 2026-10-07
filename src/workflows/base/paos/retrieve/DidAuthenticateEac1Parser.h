/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/element/Eac1InputType.h"
#include "paos/element/ElementParser.h"
#include "paos/retrieve/DidAuthenticateEac1.h"


namespace governikus
{

class DidAuthenticateEac1Parser
{
	private:
		const QSharedPointer<ElementParser> mParser;

		Eac1InputType parseEac1InputType();
		void parseCertificateDescription(Eac1InputType& pEac1, QString& pCertificateDescription);
		void parseRequiredCHAT(Eac1InputType& pEac1, QString& pRequiredCHAT);
		void parseOptionalCHAT(Eac1InputType& pEac1, QString& pOptionalCHAT);
		void parseAuthenticatedAuxiliaryData(Eac1InputType& pEac1, QString& pAuthenticatedAuxiliaryData);
		void parseTransactionInfo(Eac1InputType& pEac1, QString& pTransactionInfo);
		void parseCertificate(Eac1InputType& pEac1);
		void parseAcceptedEidType(Eac1InputType& pEac1);

	public:
		explicit DidAuthenticateEac1Parser(const QSharedPointer<ElementParser>& pParser);

		[[nodiscard]] std::unique_ptr<DIDAuthenticateEAC1> parse();
};

} // namespace governikus
