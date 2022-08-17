/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "DestroyPaceChannelCommand.h"

#include "CardConnection.h"

using namespace governikus;


DestroyPaceChannelCommand::DestroyPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: BaseCardCommand(pCardConnectionWorker)
{
}


void DestroyPaceChannelCommand::internalExecute()
{
	mReturnCode = getCardConnectionWorker()->destroyPaceChannel();
}
