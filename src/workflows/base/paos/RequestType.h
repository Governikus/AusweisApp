/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Represents a PAOS request type according to ISOCommon.xsd
 */

#pragma once

#include "PaosMessage.h"


namespace governikus
{

class RequestType
	: public PaosMessage
{
	public:
		explicit RequestType(PaosType pType);
		~RequestType() override;
};

} // namespace governikus
