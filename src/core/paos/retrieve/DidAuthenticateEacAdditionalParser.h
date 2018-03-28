/*!
 * \brief Parse information for DidAuthenticateEacAdditional.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/PaosMessage.h"
#include "paos/retrieve/DidAuthenticateEacAdditional.h"
#include "paos/retrieve/PaosParser.h"

#include <QScopedPointer>
#include <QString>


namespace governikus
{

class DidAuthenticateEacAdditionalParser
	: public PaosParser
{
	public:
		DidAuthenticateEacAdditionalParser();
		virtual ~DidAuthenticateEacAdditionalParser() override;

	protected:
		virtual PaosMessage* parseMessage() override;

	private:
		QString parseEacAdditionalInputType();

	private:
		QScopedPointer<DIDAuthenticateEACAdditional> mDidAuthenticateEacAdditional;
};

} /* namespace governikus */
