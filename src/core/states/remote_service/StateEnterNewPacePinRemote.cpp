/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterNewPacePinRemote.h"


using namespace governikus;


StateEnterNewPacePinRemote::StateEnterNewPacePinRemote(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateEnterNewPacePinRemote::run()
{
	Q_EMIT fireContinue();
}


void StateEnterNewPacePinRemote::onCancelChangePin()
{
	if (getContext() && getContext()->getRemoteServer() && getContext()->getRemoteServer()->getMessageHandler())
	{
		getContext()->setModifyPinMessageResponseApdu(ResponseApdu(StatusCode::INPUT_CANCELLED));
	}
	Q_EMIT fireAbort();
}


void StateEnterNewPacePinRemote::onEntry(QEvent* pEvent)
{
	if (getContext() && getContext()->getRemoteServer() && getContext()->getRemoteServer()->getMessageHandler())
	{
		const auto& handler = getContext()->getRemoteServer()->getMessageHandler();
		mConnections += connect(handler.data(), &ServerMessageHandler::destroyed, this, &StateEnterNewPacePinRemote::onCancelChangePin);
	}

	mConnections += connect(getContext().data(), &RemoteServiceContext::fireCancelPasswordRequest, this, &StateEnterNewPacePinRemote::onCancelChangePin);
	AbstractState::onEntry(pEvent);
}
