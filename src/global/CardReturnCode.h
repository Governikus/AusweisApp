/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "GlobalStatus.h"


namespace governikus
{

defineEnumTypeQmlExposed(CardReturnCode,
		UNDEFINED,
		OK,
		RESPONSE_EMPTY,
		CARD_NOT_FOUND,
		UNKNOWN,
		INPUT_TIME_OUT,
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
};


} // namespace governikus
