/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
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
