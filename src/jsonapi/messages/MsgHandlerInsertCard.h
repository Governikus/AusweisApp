/*!
 * \brief Message InsertCard of JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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


} /* namespace governikus */
