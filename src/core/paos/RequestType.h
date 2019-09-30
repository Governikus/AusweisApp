/*!
 * \brief Represents a PAOS request type according to ISOCommon.xsd
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
		virtual ~RequestType();
};

} // namespace governikus
