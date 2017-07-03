/*!
 * DestroyPaceChannelCommand.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "DestroyPaceChannelCommand.h"


using namespace governikus;


DestroyPaceChannelCommand::DestroyPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: BaseCardCommand(pCardConnectionWorker)
{
}


DestroyPaceChannelCommand::~DestroyPaceChannelCommand()
{
}


void DestroyPaceChannelCommand::internalExecute()
{
	mCardConnectionWorker->destroyPaceChannel();
}
