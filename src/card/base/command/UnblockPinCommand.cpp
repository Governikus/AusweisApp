/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "UnblockPinCommand.h"


using namespace governikus;


UnblockPinCommand::UnblockPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, const QString& pPuk)
	: BaseCardCommand(pCardConnectionWorker)
	, mPuk(pPuk)
{
}


UnblockPinCommand::~UnblockPinCommand()
{

}


void UnblockPinCommand::internalExecute()
{
	mReturnCode = mCardConnectionWorker->updateRetryCounter();
	if (mReturnCode != CardReturnCode::OK)
	{
		return;
	}

	if (mCardConnectionWorker->getReaderInfo().getRetryCounter() != 0 || mCardConnectionWorker->getReaderInfo().isPinDeactivated())
	{
		mReturnCode = CardReturnCode::PIN_NOT_BLOCKED;
		return;
	}

	EstablishPACEChannelOutput output;
	mReturnCode = mCardConnectionWorker->establishPaceChannel(PACE_PASSWORD_ID::PACE_PUK, mPuk, output);
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
