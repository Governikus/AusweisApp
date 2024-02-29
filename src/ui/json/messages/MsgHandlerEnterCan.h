/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message EnterCan of JSON API.
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
