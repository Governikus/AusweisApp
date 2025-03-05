/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
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
