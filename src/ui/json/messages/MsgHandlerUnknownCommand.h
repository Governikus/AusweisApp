/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief MsgHandlerUnknownCommand of JSON API.
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
