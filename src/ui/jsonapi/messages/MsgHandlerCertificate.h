/*!
 * \brief Message handler for GET_CERTIFICATE of JSON API.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerCertificate
	: public MsgHandler
{
	public:
		MsgHandlerCertificate(const MsgContext& pContext);
};


} // namespace governikus
