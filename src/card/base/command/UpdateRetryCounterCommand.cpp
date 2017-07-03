/*!
 * UpdateRetryCounterCommand.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "UpdateRetryCounterCommand.h"


using namespace governikus;


UpdateRetryCounterCommand::UpdateRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: BaseCardCommand(pCardConnectionWorker)
{
}


UpdateRetryCounterCommand::~UpdateRetryCounterCommand()
{

}


void UpdateRetryCounterCommand::internalExecute()
{
	mReturnCode = mCardConnectionWorker->updateRetryCounter();
}
