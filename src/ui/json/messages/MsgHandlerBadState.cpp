/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerBadState.h"

using namespace governikus;

MsgHandlerBadState::MsgHandlerBadState(MsgCmdType pType)
	: MsgHandler(MsgType::BAD_STATE)
{
	if (pType != MsgCmdType::UNDEFINED)
	{
		setValue(QLatin1String("error"), getEnumName(pType));
	}
}
