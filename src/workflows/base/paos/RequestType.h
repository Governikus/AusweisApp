/*!
 * \brief Represents a PAOS request type according to ISOCommon.xsd
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
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
