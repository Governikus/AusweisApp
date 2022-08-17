/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterNewPacePinIfd.h"


using namespace governikus;


StateEnterNewPacePinIfd::StateEnterNewPacePinIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setKeepCardConnectionAlive();
}


void StateEnterNewPacePinIfd::run()
{
	Q_EMIT fireContinue();
}


void StateEnterNewPacePinIfd::onCancelChangePin()
{
	if (getContext() && getContext()->getIfdServer() && getContext()->getIfdServer()->getMessageHandler())
	{
		getContext()->setModifyPinMessageResponseApdu(ResponseApdu(StatusCode::INPUT_CANCELLED));
	}
	Q_EMIT fireAbort();
}


void StateEnterNewPacePinIfd::onEntry(QEvent* pEvent)
{
	stopNfcScanIfNecessary();

	if (getContext() && getContext()->getIfdServer() && getContext()->getIfdServer()->getMessageHandler())
	{
		const auto& handler = getContext()->getIfdServer()->getMessageHandler();
		mConnections += connect(handler.data(), &ServerMessageHandler::destroyed, this, &StateEnterNewPacePinIfd::onCancelChangePin);
	}

	mConnections += connect(getContext().data(), &IfdServiceContext::fireCancelPasswordRequest, this, &StateEnterNewPacePinIfd::onCancelChangePin);
	AbstractState::onEntry(pEvent);
}
