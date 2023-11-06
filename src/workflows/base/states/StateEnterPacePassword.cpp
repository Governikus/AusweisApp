/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterPacePassword.h"

#include "VolatileSettings.h"


using namespace governikus;


StateEnterPacePassword::StateEnterPacePassword(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setKeepCardConnectionAlive();
}


void StateEnterPacePassword::run()
{
	if (getContext()->getStatus().isError())
	{
		Q_ASSERT(getContext()->getFailureCode().has_value());
		Q_EMIT firePropagateAbort();
	}
	else
	{
		Q_EMIT fireContinue();
	}
}


void StateEnterPacePassword::onEntry(QEvent* pEvent)
{
	switch (getContext()->getLastPaceResult())
	{
		case CardReturnCode::OK:
		case CardReturnCode::OK_PUK:
			stopNfcScanIfNecessary();
			break;

		default:
			const auto* volatileSettings = Env::getSingleton<VolatileSettings>();
			//: INFO IOS The current session was interrupted because of a wrong password.
			stopNfcScanIfNecessary(volatileSettings->isUsedAsSDK() ? volatileSettings->getMessages().getSessionFailed() : tr("Access denied."));
	}

	AbstractState::onEntry(pEvent);
}
