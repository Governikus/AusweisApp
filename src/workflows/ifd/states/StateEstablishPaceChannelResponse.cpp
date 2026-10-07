/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
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
	const auto& paceOutput = context->getPaceOutput();

	const auto& ifdServer = context->getIfdServer();
	if (ifdServer)
	{
		const auto& messageHandler = ifdServer->getMessageHandler();
		if (messageHandler)
		{
			Q_ASSERT(!context->getSlotHandle().isEmpty());

			messageHandler->sendEstablishPaceChannelResponse(
					context->getSlotHandle(),
					paceOutput
					);
		}
	}

	if (paceOutput.wrongPasswordUsed())
	{
		Q_EMIT fireWrongPacePassword();
		return;
	}

	Q_EMIT fireContinue();
}
