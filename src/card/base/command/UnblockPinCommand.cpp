/*!
 * UnblockPinCommand.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
	mReturnCode = mCardConnectionWorker->establishPaceChannel(PACE_PIN_ID::PACE_PUK, mPuk, output);
	if (mReturnCode != CardReturnCode::OK)
	{
		return;
	}

	// unblock PIN (reset retry counter)
	ResponseApdu response;
	mReturnCode = mCardConnectionWorker->transmit(ResetRetryCounterBuilder().build(), response);
	if (mReturnCode == CardReturnCode::OK && response.getSW1() == Enum<SW1>::getValue(SW1::ERROR_COMMAND_NOT_ALLOWED))
	{
		mReturnCode = CardReturnCode::PUK_INOPERATIVE;
	}
}
