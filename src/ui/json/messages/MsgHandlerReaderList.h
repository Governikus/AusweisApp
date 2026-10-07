/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerReaderList
	: public MsgHandler
{
	public:
		explicit MsgHandlerReaderList(const MsgContext& pContext);
};


} // namespace governikus
