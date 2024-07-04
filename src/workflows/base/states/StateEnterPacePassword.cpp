/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterPacePassword.h"

#include "VolatileSettings.h"


using namespace governikus;


StateEnterPacePassword::StateEnterPacePassword(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setKeepCardConnectionAlive();
	setHandleNfcStop();
}


void StateEnterPacePassword::run()
{
	Q_EMIT fireContinue();
}


void StateEnterPacePassword::onEntry(QEvent* pEvent)
{
	switch (getContext()->getLastPaceResult())
	{
		case CardReturnCode::OK:
		case CardReturnCode::OK_PUK:
		case CardReturnCode::OK_CAN:
			stopNfcScanIfNecessary();
			break;

		default:
			const auto* volatileSettings = Env::getSingleton<VolatileSettings>();
			//: INFO IOS The current session was interrupted because of a wrong password.
			stopNfcScanIfNecessary(volatileSettings->isUsedAsSDK() ? volatileSettings->getMessages().getSessionFailed() : tr("Access denied."));
	}

	AbstractState::onEntry(pEvent);
}
