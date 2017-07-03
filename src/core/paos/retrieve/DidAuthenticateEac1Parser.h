/*!
 * DidAuthenticateEac1Parser.h
 *
 * \brief Parser for the PAOS DidAuthenticateEac1 element.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QScopedPointer>
#include <QXmlStreamReader>

#include "paos/retrieve/DidAuthenticateEac1.h"
#include "paos/retrieve/PaosParser.h"

namespace governikus
{

class DidAuthenticateEac1Parser
	: public PaosParser
{
	public:
		DidAuthenticateEac1Parser();
		~DidAuthenticateEac1Parser();

	protected:
		virtual PaosMessage* parseMessage() override;

	private:
		Eac1InputType parseEac1InputType();

	private:
		QScopedPointer<DIDAuthenticateEAC1> mDidAuthenticateEac1;
};

}
