/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message PAUSE of JSON API.
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
