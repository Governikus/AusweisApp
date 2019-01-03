/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerUnknownCommand.h"

using namespace governikus;

MsgHandlerUnknownCommand::MsgHandlerUnknownCommand(const QString& pName)
	: MsgHandler(MsgType::UNKNOWN_COMMAND, "error", pName)
{
}
