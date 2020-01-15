/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "UnblockPinCommand.h"

#include "ResetRetryCounterBuilder.h"

using namespace governikus;


UnblockPinCommand::UnblockPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, const QString& pPuk)
	: BaseCardCommand(pCardConnectionWorker)
	, mPuk(pPuk)
{
}


void UnblockPinCommand::internalExecute()
{
	if (!mCardConnectionWorker->getReaderInfo().hasEidCard())
	{
		mReturnCode = CardReturnCode::CARD_NOT_FOUND;
		return;
	}

	Q_ASSERT(mCardConnectionWorker->getReaderInfo().getRetryCounter() == 0);
	if (mCardConnectionWorker->getReaderInfo().getRetryCounter() > 0 || mCardConnectionWorker->getReaderInfo().isPinDeactivated())
	{
		mReturnCode = CardReturnCode::PIN_NOT_BLOCKED;
		return;
	}

	mReturnCode = mCardConnectionWorker->establishPaceChannel(PacePasswordId::PACE_PUK, mPuk).getPaceReturnCode();
	if (mReturnCode != CardReturnCode::OK)
	{
		return;
	}

	// unblock PIN (reset retry counter)
	auto [returnCode, response] = mCardConnectionWorker->transmit(ResetRetryCounterBuilder().build());
	mReturnCode = returnCode;
	if (mReturnCode == CardReturnCode::OK && response.getSW1() == SW1::ERROR_COMMAND_NOT_ALLOWED)
	{
		mCardConnectionWorker->setPukInoperative();
		mReturnCode = CardReturnCode::PUK_INOPERATIVE;
	}
}
