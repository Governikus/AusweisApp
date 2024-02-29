/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "ResetRetryCounterCommand.h"

using namespace governikus;


ResetRetryCounterCommand::ResetRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: BaseCardCommand(pCardConnectionWorker)
{
}


void ResetRetryCounterCommand::internalExecute()
{
	const CommandApdu cmdApdu(Ins::RESET_RETRY_COUNTER, CommandApdu::UNBLOCK, CommandApdu::PIN);
	auto [returnCode, response] = getCardConnectionWorker()->transmit(cmdApdu);
	setReturnCode(returnCode);
	if (getReturnCode() == CardReturnCode::OK && response.getSW1() == SW1::ERROR_COMMAND_NOT_ALLOWED)
	{
		getCardConnectionWorker()->setPukInoperative();
		setReturnCode(CardReturnCode::PUK_INOPERATIVE);
	}
}
