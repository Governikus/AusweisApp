/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

#include "ResetRetryCounterCommand.h"


using namespace governikus;


ResetRetryCounterCommand::ResetRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: BaseCardCommand(pCardConnectionWorker)
	, mStatusCode(StatusCode::UNKNOWN)
{
}


void ResetRetryCounterCommand::internalExecute()
{
	const CommandApdu cmdApdu(Ins::RESET_RETRY_COUNTER, CommandApdu::UNBLOCK, CommandApdu::PIN);
	auto [returnCode, response] = getCardConnectionWorker()->transmit(cmdApdu);
	setReturnCode(returnCode);

	if (getReturnCode() != CardReturnCode::OK)
	{
		return;
	}

	mStatusCode = response.getStatusCode();
	if (mStatusCode == StatusCode::ACCESS_DENIED)
	{
		getCardConnectionWorker()->setPukInoperative();
	}
}


StatusCode ResetRetryCounterCommand::getStatusCode() const
{
	return mStatusCode;
}
