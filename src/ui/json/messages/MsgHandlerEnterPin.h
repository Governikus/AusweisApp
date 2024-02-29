/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message EnterPin of JSON API.
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandlerEnterNumber.h"

namespace governikus
{

class MsgHandlerEnterPin
	: public MsgHandlerEnterNumber
{
	public:
		explicit MsgHandlerEnterPin(const MsgContext& pContext);
		explicit MsgHandlerEnterPin(const QJsonObject& pObj, MsgContext& pContext);
};


} // namespace governikus
