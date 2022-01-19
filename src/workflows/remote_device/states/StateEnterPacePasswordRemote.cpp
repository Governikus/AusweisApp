/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterPacePasswordRemote.h"


using namespace governikus;


StateEnterPacePasswordRemote::StateEnterPacePasswordRemote(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateEnterPacePasswordRemote::run()
{
	Q_EMIT fireContinue();
}


void StateEnterPacePasswordRemote::onCancelEstablishPaceChannel()
{
	if (getContext() && getContext()->getRemoteServer() && getContext()->getRemoteServer()->getMessageHandler())
	{
		EstablishPaceChannelOutput channelOutput = EstablishPaceChannelOutput();
		channelOutput.setPaceReturnCode(CardReturnCode::CANCELLATION_BY_USER);
		getContext()->setEstablishPaceChannelOutput(channelOutput);
	}

	Q_EMIT fireAbort();
}


void StateEnterPacePasswordRemote::onEntry(QEvent* pEvent)
{
	stopScanIfNecessary();

	if (getContext() && getContext()->getRemoteServer() && getContext()->getRemoteServer()->getMessageHandler())
	{
		const auto& handler = getContext()->getRemoteServer()->getMessageHandler();
		mConnections += connect(handler.data(), &ServerMessageHandler::destroyed, this, &StateEnterPacePasswordRemote::onCancelEstablishPaceChannel);
	}

	mConnections += connect(getContext().data(), &RemoteServiceContext::fireCancelPasswordRequest, this, &StateEnterPacePasswordRemote::onCancelEstablishPaceChannel);
	AbstractState::onEntry(pEvent);
}
