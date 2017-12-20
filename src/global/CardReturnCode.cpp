/*
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "CardReturnCode.h"
#include "moc_CardReturnCode.cpp"

#include "GlobalStatus.h"

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
			return GlobalStatus::Code::Card_Not_Found;

		case CardReturnCode::COMMAND_FAILED:
			return GlobalStatus::Code::Card_Communication_Error;

		case CardReturnCode::PROTOCOL_ERROR:
		case CardReturnCode::UNEXPECTED_TRANSMIT_STATUS:
			return GlobalStatus::Code::Card_Protocol_Error;

		case CardReturnCode::CANCELLATION_BY_USER:
			return GlobalStatus::Code::Card_Cancellation_By_User;

		case CardReturnCode::INPUT_TIME_OUT:
			return GlobalStatus::Code::Card_Input_TimeOut;

		case CardReturnCode::INVALID_PIN:
			return GlobalStatus::Code::Card_Invalid_Pin;

		case CardReturnCode::INVALID_CAN:
			return GlobalStatus::Code::Card_Invalid_Can;

		case CardReturnCode::INVALID_PUK:
			return GlobalStatus::Code::Card_Invalid_Puk;

		case CardReturnCode::NEW_PIN_MISMATCH:
			return GlobalStatus::Code::Card_NewPin_Mismatch;

		case CardReturnCode::NEW_PIN_INVALID_LENGTH:
			return GlobalStatus::Code::Card_NewPin_Invalid_Length;

		case CardReturnCode::PIN_BLOCKED:
			return GlobalStatus::Code::Card_Pin_Blocked;

		case CardReturnCode::PIN_NOT_BLOCKED:
			return GlobalStatus::Code::Card_Pin_Not_Blocked;

		case CardReturnCode::PUK_INOPERATIVE:
			return GlobalStatus::Code::Card_Puk_Blocked;
	}

	Q_UNREACHABLE();
}
