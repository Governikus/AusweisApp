/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateChangePinResponse.h"

#include "ServerMessageHandler.h"
#include "pinpad/PinModify.h"

using namespace governikus;


StateChangePinResponse::StateChangePinResponse(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateChangePinResponse::run()
{
	const QSharedPointer<IfdServiceContext>& context = getContext();
	const auto& responseApdu = context->getModifyPinMessageResponseApdu();

	const auto& ifdServer = context->getIfdServer();
	if (ifdServer)
	{
		const auto& messageHandler = ifdServer->getMessageHandler();
		if (messageHandler)
		{
			Q_ASSERT(!context->getModifyPinMessage()->getSlotHandle().isEmpty());

			messageHandler->sendModifyPinResponse(
					context->getModifyPinMessage()->getSlotHandle(),
					responseApdu
					);
		}
	}

	if (responseApdu.getStatusCode() == StatusCode::SUCCESS)
	{
		Q_EMIT firePacePasswordModified();
		return;
	}

	Q_EMIT fireContinue();
}
