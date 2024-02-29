/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message BadState of JSON API.
 */

#pragma once

#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerBadState
	: public MsgHandler
{
	public:
		explicit MsgHandlerBadState(MsgCmdType pType = MsgCmdType::UNDEFINED);
};


} // namespace governikus
