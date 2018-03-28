/*!
 * \brief Parse information for DidAuthenticateEac2.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/element/Eac2InputType.h"
#include "paos/PaosMessage.h"
#include "paos/retrieve/DidAuthenticateEac2.h"
#include "paos/retrieve/PaosParser.h"

#include <QScopedPointer>


namespace governikus
{

class DidAuthenticateEac2Parser
	: public PaosParser
{
	public:
		DidAuthenticateEac2Parser();
		virtual ~DidAuthenticateEac2Parser() override;

	protected:
		virtual PaosMessage* parseMessage() override;

	private:
		Eac2InputType parseEac2InputType();

	private:
		QScopedPointer<DIDAuthenticateEAC2> mDidAuthenticateEac2;
};

} /* namespace governikus */
