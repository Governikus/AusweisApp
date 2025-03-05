/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateEstablishPaceChannelResponse.h"

#include "ServerMessageHandler.h"

using namespace governikus;


StateEstablishPaceChannelResponse::StateEstablishPaceChannelResponse(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateEstablishPaceChannelResponse::run()
{
	const QSharedPointer<IfdServiceContext>& context = getContext();
	const auto& establishPaceChannelOutput = context->getEstablishPaceChannelOutput();

	const auto& ifdServer = context->getIfdServer();
	if (ifdServer)
	{
		const auto& messageHandler = ifdServer->getMessageHandler();
		if (messageHandler)
		{
			Q_ASSERT(!context->getSlotHandle().isEmpty());

			messageHandler->sendEstablishPaceChannelResponse(
					context->getSlotHandle(),
					establishPaceChannelOutput
					);
		}
	}

	const bool isWrongPacePassword = CardReturnCodeUtil::equalsWrongPacePassword(establishPaceChannelOutput.getPaceReturnCode());
	context->setEstablishPaceChannel(nullptr);

	if (isWrongPacePassword)
	{
		Q_EMIT fireWrongPacePassword();
		return;
	}

	Q_EMIT fireContinue();
}
