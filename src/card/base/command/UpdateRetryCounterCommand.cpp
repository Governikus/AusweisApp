/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "UpdateRetryCounterCommand.h"

using namespace governikus;


UpdateRetryCounterCommand::UpdateRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, const QString& pSlotHandle)
	: BaseCardCommand(pCardConnectionWorker)
	, mSlotHandle(pSlotHandle)
{
}


void UpdateRetryCounterCommand::internalExecute()
{
	setReturnCode(getCardConnectionWorker()->updateRetryCounter());
}


const QString& UpdateRetryCounterCommand::getSlotHandle() const
{
	return mSlotHandle;
}
