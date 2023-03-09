/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message handler for GET_CERTIFICATE of JSON API.
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
		explicit MsgHandlerCertificate(const MsgContext& pContext);
};


} // namespace governikus
