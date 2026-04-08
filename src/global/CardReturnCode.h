/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "GlobalStatus.h"


namespace governikus
{

defineEnumTypeQmlExposed(CardReturnCode,
		UNDEFINED,
		OK,
		OK_PUK,
		OK_CAN,
		RESPONSE_EMPTY,
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
		PIN_NOT_BLOCKED,
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
