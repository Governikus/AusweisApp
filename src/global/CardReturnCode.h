/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Global error code definitions
 */

#pragma once

#include "EnumHelper.h"
#include "GlobalStatus.h"

namespace governikus
{

defineEnumType(CardReturnCode,
		UNDEFINED,
		OK,
		OK_PUK,
		RETRY_ALLOWED,
		CARD_NOT_FOUND,
		UNKNOWN,
		INPUT_TIME_OUT,
		INVALID_CAN,
		INVALID_PASSWORD,
		INVALID_PIN,
		INVALID_PIN_2,
		INVALID_PIN_3,
		INVALID_PUK,
		COMMAND_FAILED,
		CANCELLATION_BY_USER,
		NEW_PIN_MISMATCH,
		NEW_PIN_INVALID_LENGTH,
		PIN_BLOCKED,
		PIN_NOT_BLOCKED,
		PUK_INOPERATIVE,
		PROTOCOL_ERROR,
		WRONG_LENGTH,
		UNEXPECTED_TRANSMIT_STATUS)


class CardReturnCodeUtil
{
	private:
		CardReturnCodeUtil() = default;

	public:
		static GlobalStatus toGlobalStatus(CardReturnCode pCode);
		static bool equalsWrongPacePassword(CardReturnCode pCode);
};

} // namespace governikus
