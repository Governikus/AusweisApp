/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StateChangePinResponse.h"

#include "PinModify.h"
#include "ServerMessageHandler.h"

using namespace governikus;


StateChangePinResponse::StateChangePinResponse(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateChangePinResponse::run()
{
	const QSharedPointer<RemoteServiceContext>& context = getContext();
	const auto& responseApdu = context->getModifyPinMessageResponseApdu();

	const auto& remoteServer = context->getRemoteServer();
	if (remoteServer)
	{
		const auto& messageHandler = remoteServer->getMessageHandler();
		if (messageHandler)
		{
			Q_ASSERT(!context->getModifyPinMessage()->getSlotHandle().isEmpty());

			messageHandler->sendModifyPinResponse(
					context->getModifyPinMessage()->getSlotHandle(),
					responseApdu
					);
		}
	}

	if (responseApdu.getCardReturnCode() == CardReturnCode::OK)
	{
		Q_EMIT firePacePasswordModified();
		return;
	}

	Q_EMIT fireContinue();
}
