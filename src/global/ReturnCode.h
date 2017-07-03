/*!
 * ReturnCode.h
 *
 * \brief Global error code definitions
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{

defineEnumType(ReturnCode,
		UNDEFINED,
		OK,
		NO_CARD,
		UNKNOWN,
		TIME_OUT,
		CAN_INVALID,
		PIN_INVALID,
		PUK_INVALID,
		COMMAND_FAILED,
		GET_CHALLENGE_FAILED,
		CANCELLATION_BY_USER,
		NEW_PINS_DONT_MATCH,
		NEW_PIN_TOO_SHORT_OR_LONG,
		PIN_BLOCKED,
		PIN_NOT_BLOCKED,
		PUK_INOPERATIVE,
		PROTOCOL_ERROR,
		UNEXPECTED_TRANSMIT_STATUS)

} /* namespace governikus */
