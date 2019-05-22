/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "UnblockPinCommand.h"

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

	EstablishPaceChannelOutput output;
	mReturnCode = mCardConnectionWorker->establishPaceChannel(PacePasswordId::PACE_PUK, mPuk, output);
	if (mReturnCode != CardReturnCode::OK)
	{
		return;
	}

	// unblock PIN (reset retry counter)
	ResponseApdu response;
	mReturnCode = mCardConnectionWorker->transmit(ResetRetryCounterBuilder().build(), response);
	if (mReturnCode == CardReturnCode::OK && response.getSW1() == SW1::ERROR_COMMAND_NOT_ALLOWED)
	{
		mCardConnectionWorker->setPukInoperative();
		mReturnCode = CardReturnCode::PUK_INOPERATIVE;
	}
}
