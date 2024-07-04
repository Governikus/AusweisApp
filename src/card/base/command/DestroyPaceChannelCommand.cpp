/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "DestroyPaceChannelCommand.h"

using namespace governikus;


DestroyPaceChannelCommand::DestroyPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		const QString& pSlotHandle)
	: BaseCardCommand(pCardConnectionWorker)
	, mSlotHandle(pSlotHandle)
	, mSecureMessagingStopped(false)
{
	connect(pCardConnectionWorker.data(), &CardConnectionWorker::fireSecureMessagingStopped, this, [this](){
			mSecureMessagingStopped = true;
		});
}


void DestroyPaceChannelCommand::internalExecute()
{
	setReturnCode(getCardConnectionWorker()->destroyPaceChannel());
}
