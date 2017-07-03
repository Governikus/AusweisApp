/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerBadState.h"

using namespace governikus;

MsgHandlerBadState::MsgHandlerBadState(MsgCmdType pType)
	: MsgHandler(MsgType::BAD_STATE)
{
	if (pType != MsgCmdType::UNDEFINED)
	{
		mJsonObject["error"] = getEnumName(pType);
	}
}
