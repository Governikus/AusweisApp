/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandlerEnterNumber.h"

namespace governikus
{

class MsgHandlerEnterNewPin
	: public MsgHandlerEnterNumber
{
	public:
		explicit MsgHandlerEnterNewPin(const MsgContext& pContext);
		explicit MsgHandlerEnterNewPin(const QJsonObject& pObj, MsgContext& pContext);
};


} // namespace governikus
