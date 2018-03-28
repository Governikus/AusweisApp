/*!
 * \brief Enumerations of message types and additional stuff.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{
defineEnumType(MsgLevel, v1 = 1) // See MsgHandler::DEFAULT_MSG_LEVEL

defineEnumType(MsgType,
		INVALID,
		UNKNOWN_COMMAND,
		INTERNAL_ERROR,
		INFO,
		API_LEVEL,
		READER,
		READER_LIST,
		BAD_STATE,
		AUTH,
		CERTIFICATE,
		ACCESS_RIGHTS,
		INSERT_CARD,
		ENTER_PIN,
		ENTER_CAN,
		ENTER_PUK)

defineEnumType(MsgCmdType,
		UNDEFINED,
		ACCEPT,
		CANCEL,
		GET_INFO,
		GET_API_LEVEL,
		SET_API_LEVEL,
		GET_READER,
		GET_READER_LIST,
		RUN_AUTH,
		GET_CERTIFICATE,
		GET_ACCESS_RIGHTS,
		SET_ACCESS_RIGHTS,
		SET_PIN,
		SET_CAN,
		SET_PUK)

} /* namespace governikus */
