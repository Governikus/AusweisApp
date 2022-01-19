/*!
 * \brief MsgHandlerUnknownCommand of JSON API.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerUnknownCommand
	: public MsgHandler
{
	public:
		explicit MsgHandlerUnknownCommand(const QString& pName);
		explicit MsgHandlerUnknownCommand(const MsgCmdType& pCmdType);
};


} // namespace governikus
