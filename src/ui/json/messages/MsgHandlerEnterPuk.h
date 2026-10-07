/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandlerEnterNumber.h"

namespace governikus
{

class MsgHandlerEnterPuk
	: public MsgHandlerEnterNumber
{
	public:
		explicit MsgHandlerEnterPuk(const MsgContext& pContext);
		explicit MsgHandlerEnterPuk(const QJsonObject& pObj, MsgContext& pContext);
};


} // namespace governikus
