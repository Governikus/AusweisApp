/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "UpdateRetryCounterCommand.h"

using namespace governikus;


UpdateRetryCounterCommand::UpdateRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: BaseCardCommand(pCardConnectionWorker)
{
}


void UpdateRetryCounterCommand::internalExecute()
{
	mReturnCode = mCardConnectionWorker->updateRetryCounter();
}
