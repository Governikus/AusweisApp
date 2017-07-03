/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerUnknownCommand.h"

using namespace governikus;

MsgHandlerUnknownCommand::MsgHandlerUnknownCommand(const QString& pName)
	: MsgHandler(MsgType::UNKNOWN_COMMAND, "error", pName)
{
}
