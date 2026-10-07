/**
 * Copyright (c) 2024-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerPause
	: public MsgHandler
{
	private:
		void setCause(const QLatin1String pCause);

	public:
		explicit MsgHandlerPause();
		explicit MsgHandlerPause(MsgContext& pContext);
};

} // namespace governikus
