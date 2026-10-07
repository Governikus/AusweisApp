/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandlerEnterNumber.h"

namespace governikus
{

class MsgHandlerEnterCan
	: public MsgHandlerEnterNumber
{
	public:
		explicit MsgHandlerEnterCan(const MsgContext& pContext);
		explicit MsgHandlerEnterCan(const QJsonObject& pObj, MsgContext& pContext);
};


} // namespace governikus
