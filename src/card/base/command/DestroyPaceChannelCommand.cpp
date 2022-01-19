/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "DestroyPaceChannelCommand.h"


using namespace governikus;


DestroyPaceChannelCommand::DestroyPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: BaseCardCommand(pCardConnectionWorker)
{
}


void DestroyPaceChannelCommand::internalExecute()
{
	mReturnCode = mCardConnectionWorker->destroyPaceChannel();
}
