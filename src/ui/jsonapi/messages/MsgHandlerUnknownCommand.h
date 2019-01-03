/*!
 * \brief MsgHandlerUnknownCommand of JSON API.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerUnknownCommand
	: public MsgHandler
{
	public:
		MsgHandlerUnknownCommand(const QString& pName);
};


} // namespace governikus
