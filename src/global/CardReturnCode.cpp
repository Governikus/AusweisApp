/*
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "CardReturnCode.h"

#include "Initializer.h"
#include "moc_CardReturnCode.cpp"

using namespace governikus;


static Initializer::Entry X([] {
			qRegisterMetaType<CardReturnCode>("CardReturnCode");
		});


GlobalStatus CardReturnCodeUtil::toGlobalStatus(CardReturnCode pCode)
{
	switch (pCode)
	{
		case CardReturnCode::OK:
		case CardReturnCode::OK_PUK:
			return GlobalStatus::Code::No_Error;

		case CardReturnCode::UNDEFINED:
		case CardReturnCode::UNKNOWN:
			return GlobalStatus::Code::Unknown_Error;

		case CardReturnCode::CARD_NOT_FOUND:
		case CardReturnCode::RETRY_ALLOWED:
			return GlobalStatus::Code::Card_Not_Found;

		case CardReturnCode::COMMAND_FAILED:
			return GlobalStatus::Code::Card_Communication_Error;

		case CardReturnCode::PROTOCOL_ERROR:
			return GlobalStatus::Code::Card_Protocol_Error;

		case CardReturnCode::UNEXPECTED_TRANSMIT_STATUS:
			return GlobalStatus::Code::Card_Unexpected_Transmit_Status;

		case CardReturnCode::CANCELLATION_BY_USER:
			return GlobalStatus::Code::Card_Cancellation_By_User;

		case CardReturnCode::INPUT_TIME_OUT:
			return GlobalStatus::Code::Card_Input_TimeOut;

		case CardReturnCode::INVALID_PIN:
		case CardReturnCode::INVALID_PIN_2:
		case CardReturnCode::INVALID_PIN_3:
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
	return GlobalStatus::Code::Unknown_Error;
}


bool CardReturnCodeUtil::equalsWrongPacePassword(CardReturnCode pCode)
{
	switch (pCode)
	{
		case CardReturnCode::INVALID_CAN:
		case CardReturnCode::INVALID_PIN:
		case CardReturnCode::INVALID_PIN_2:
		case CardReturnCode::INVALID_PIN_3:
		case CardReturnCode::INVALID_PUK:
		case CardReturnCode::NEW_PIN_MISMATCH:
		case CardReturnCode::NEW_PIN_INVALID_LENGTH:
		case CardReturnCode::PIN_NOT_BLOCKED:
		case CardReturnCode::PIN_BLOCKED:
			return true;

		case CardReturnCode::UNDEFINED:
		case CardReturnCode::RETRY_ALLOWED:
		case CardReturnCode::CARD_NOT_FOUND:
		case CardReturnCode::UNKNOWN:
		case CardReturnCode::COMMAND_FAILED:
		case CardReturnCode::PROTOCOL_ERROR:
		case CardReturnCode::UNEXPECTED_TRANSMIT_STATUS:
		case CardReturnCode::OK:
		case CardReturnCode::OK_PUK:
		case CardReturnCode::CANCELLATION_BY_USER:
		case CardReturnCode::PUK_INOPERATIVE:
		case CardReturnCode::INPUT_TIME_OUT:
			return false;
	}

	Q_UNREACHABLE();
	return true;
}
