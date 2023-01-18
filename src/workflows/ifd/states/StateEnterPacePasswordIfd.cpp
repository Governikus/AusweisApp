/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterPacePasswordIfd.h"


using namespace governikus;


StateEnterPacePasswordIfd::StateEnterPacePasswordIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setKeepCardConnectionAlive();
}


void StateEnterPacePasswordIfd::run()
{
	Q_EMIT fireContinue();
}


void StateEnterPacePasswordIfd::onCancelEstablishPaceChannel(bool pRequestPinChange)
{
	if (getContext() && getContext()->getIfdServer() && getContext()->getIfdServer()->getMessageHandler())
	{
		EstablishPaceChannelOutput channelOutput;
		channelOutput.setPaceReturnCode(pRequestPinChange ? CardReturnCode::NO_ACTIVE_PIN_SET : CardReturnCode::CANCELLATION_BY_USER);
		getContext()->setEstablishPaceChannelOutput(channelOutput);
	}

	Q_EMIT fireAbort();
}


void StateEnterPacePasswordIfd::onEntry(QEvent* pEvent)
{
	stopNfcScanIfNecessary();

	if (getContext() && getContext()->getIfdServer() && getContext()->getIfdServer()->getMessageHandler())
	{
		const auto& handler = getContext()->getIfdServer()->getMessageHandler();
		mConnections += connect(handler.data(), &ServerMessageHandler::destroyed, this, &StateEnterPacePasswordIfd::onCancelEstablishPaceChannel);
	}

	mConnections += connect(getContext().data(), &IfdServiceContext::fireCancelPasswordRequest, this, &StateEnterPacePasswordIfd::onCancelEstablishPaceChannel);
	AbstractState::onEntry(pEvent);
}
