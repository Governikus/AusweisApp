/*!
 * \brief MsgHandlerUnknownCommand of JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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


} /* namespace governikus */
