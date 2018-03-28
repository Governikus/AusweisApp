/*!
 * \brief Message EnterCan of JSON API.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
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
		MsgHandlerEnterCan(const MsgContext& pContext);
		MsgHandlerEnterCan(const QJsonObject& pObj, MsgContext& pContext);
};


} /* namespace governikus */
