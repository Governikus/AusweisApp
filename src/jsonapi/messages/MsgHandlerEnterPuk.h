/*!
 * \brief Message EnterPuk of JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
		MsgHandlerEnterPuk(const MsgContext& pContext);
		MsgHandlerEnterPuk(const QJsonObject& pObj, MsgContext& pContext);
};


} /* namespace governikus */
