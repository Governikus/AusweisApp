/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerBadState.h"

using namespace governikus;

MsgHandlerBadState::MsgHandlerBadState(MsgCmdType pType)
	: MsgHandler(MsgType::BAD_STATE)
{
	if (pType != MsgCmdType::UNDEFINED)
	{
		mJsonObject[QLatin1String("error")] = getEnumName(pType);
	}
}
