/*!
 * \brief Message EnterPin of JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
		MsgHandlerEnterPin(const MsgContext& pContext);
		MsgHandlerEnterPin(const QJsonObject& pObj, MsgContext& pContext);
};


} /* namespace governikus */
