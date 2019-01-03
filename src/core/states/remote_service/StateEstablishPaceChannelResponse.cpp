/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateEstablishPaceChannelResponse.h"

#include "ServerMessageHandler.h"

using namespace governikus;


StateEstablishPaceChannelResponse::StateEstablishPaceChannelResponse(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateEstablishPaceChannelResponse::run()
{
	Q_ASSERT(getContext()->getRemoteServer());
	Q_ASSERT(getContext()->getEstablishPaceChannelMessage());

	const QSharedPointer<RemoteServiceContext>& context = getContext();
	const auto& establishPaceChannelOutput = context->getEstablishPaceChannelOutput();
	if (context->getRemoteServer() && context->getRemoteServer()->getMessageHandler())
	{
		context->getRemoteServer()->getMessageHandler()->sendEstablishPaceChannelResponse(
				context->getEstablishPaceChannelMessage()->getSlotHandle(),
				establishPaceChannelOutput
				);
	}

	const bool isWrongPacePassword = CardReturnCodeUtil::equalsWrongPacePassword(establishPaceChannelOutput.getPaceReturnCode());
	if (isWrongPacePassword)
	{
		Q_EMIT fireWrongPacePassword();
		return;
	}

	Q_EMIT fireContinue();
}
