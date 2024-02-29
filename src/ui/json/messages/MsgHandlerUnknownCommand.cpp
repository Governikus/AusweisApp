/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerUnknownCommand.h"

#include "EnumHelper.h"

using namespace governikus;

MsgHandlerUnknownCommand::MsgHandlerUnknownCommand(const QString& pName)
	: MsgHandler(MsgType::UNKNOWN_COMMAND, "error", pName)
{
}


MsgHandlerUnknownCommand::MsgHandlerUnknownCommand(const MsgCmdType& pCmdType)
	: MsgHandlerUnknownCommand(getEnumName(pCmdType))
{
}
