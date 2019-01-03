/*!
 * \brief Message InsertCard of JSON API.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerInsertCard
	: public MsgHandler
{
	public:
		MsgHandlerInsertCard(MsgContext& pContext);
};


} // namespace governikus
