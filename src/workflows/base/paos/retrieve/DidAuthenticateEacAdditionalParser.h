/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Parse information for DidAuthenticateEacAdditional.
 */

#pragma once

#include "paos/PaosMessage.h"
#include "paos/retrieve/DidAuthenticateEacAdditional.h"
#include "paos/retrieve/PaosParser.h"

#include <QString>

#include <memory>

namespace governikus
{

class DidAuthenticateEacAdditionalParser
	: public PaosParser
{
	public:
		DidAuthenticateEacAdditionalParser();
		~DidAuthenticateEacAdditionalParser() override = default;

	protected:
		PaosMessage* parseMessage() override;

	private:
		QString parseEacAdditionalInputType();

	private:
		std::unique_ptr<DIDAuthenticateEACAdditional> mDidAuthenticateEacAdditional;
};

} // namespace governikus
