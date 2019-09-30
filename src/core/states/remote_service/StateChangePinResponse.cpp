/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
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
	Q_ASSERT(getContext()->getRemoteServer());
	Q_ASSERT(getContext()->getModifyPinMessage());

	const QSharedPointer<RemoteServiceContext>& context = getContext();
	const auto& responseApdu = context->getModifyPinMessageResponseApdu();
	if (context->getRemoteServer() && context->getRemoteServer()->getMessageHandler())
	{
		context->getRemoteServer()->getMessageHandler()->sendModifyPinResponse(
				context->getModifyPinMessage()->getSlotHandle(),
				responseApdu
				);
	}

	if (responseApdu.getCardReturnCode() == CardReturnCode::OK)
	{
		Q_EMIT firePacePasswordModified();
		return;
	}

	Q_EMIT fireContinue();
}
