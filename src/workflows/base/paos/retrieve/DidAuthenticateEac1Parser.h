/*!
 * \brief Parser for the PAOS DidAuthenticateEac1 element.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/retrieve/DidAuthenticateEac1.h"
#include "paos/retrieve/PaosParser.h"

#include <memory>

namespace governikus
{

class DidAuthenticateEac1Parser
	: public PaosParser
{
	public:
		DidAuthenticateEac1Parser();

	protected:
		PaosMessage* parseMessage() override;

	private:
		Eac1InputType parseEac1InputType();
		void parseCertificateDescription(Eac1InputType& pEac1, QString& pCertificateDescription);
		void parseRequiredCHAT(Eac1InputType& pEac1, QString& pRequiredCHAT);
		void parseOptionalCHAT(Eac1InputType& pEac1, QString& pOptionalCHAT);
		void parseAuthenticatedAuxiliaryData(Eac1InputType& pEac1, QString& pAuthenticatedAuxiliaryData);
		void parseTransactionInfo(Eac1InputType& pEac1, QString& pTransactionInfo);
		void parseCertificate(Eac1InputType& pEac1);
		void parseAcceptedEidType(Eac1InputType& pEac1);

	private:
		std::unique_ptr<DIDAuthenticateEAC1> mDidAuthenticateEac1;
};

} // namespace governikus
