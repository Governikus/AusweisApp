/*!
 * \brief Message InsertCard of JSON API.
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
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
		explicit MsgHandlerInsertCard(MsgContext& pContext);
};


} // namespace governikus
