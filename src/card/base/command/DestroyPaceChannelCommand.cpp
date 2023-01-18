/*!
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
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
	setReturnCode(getCardConnectionWorker()->destroyPaceChannel());
}
