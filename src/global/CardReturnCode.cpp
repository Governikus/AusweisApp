/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "CardReturnCode.h"

#include "moc_CardReturnCode.cpp"


using namespace governikus;


GlobalStatus CardReturnCodeUtil::toGlobalStatus(CardReturnCode pCode)
{
	switch (pCode)
	{
		case CardReturnCode::OK:
			return GlobalStatus::Code::No_Error;

		case CardReturnCode::UNDEFINED:
		case CardReturnCode::UNKNOWN:
			return GlobalStatus::Code::Unknown_Error;

		case CardReturnCode::CARD_NOT_FOUND:
		case CardReturnCode::RESPONSE_EMPTY:
			return GlobalStatus::Code::Card_Not_Found;

		case CardReturnCode::COMMAND_FAILED:
			return GlobalStatus::Code::Card_Communication_Error;

		case CardReturnCode::PROTOCOL_ERROR:
			return GlobalStatus::Code::Card_Protocol_Error;

		case CardReturnCode::WRONG_LENGTH:
			return GlobalStatus::Code::Workflow_Wrong_Length_Error;

		case CardReturnCode::UNEXPECTED_TRANSMIT_STATUS:
			return GlobalStatus::Code::Card_Unexpected_Transmit_Status;

		case CardReturnCode::CANCELLATION_BY_USER:
			return GlobalStatus::Code::Card_Cancellation_By_User;

		case CardReturnCode::INPUT_TIME_OUT:
			return GlobalStatus::Code::Card_Input_TimeOut;

		case CardReturnCode::PIN_NOT_BLOCKED:
			return GlobalStatus::Code::Card_Pin_Not_Blocked;
	}

	Q_UNREACHABLE();
}
