/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#include "StateEnterPacePassword.h"

#include "VolatileSettings.h"


using namespace governikus;


StateEnterPacePassword::StateEnterPacePassword(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setHandleNfcStop();
}


void StateEnterPacePassword::run()
{
	Q_EMIT fireContinue();
}


void StateEnterPacePassword::onEntry(QEvent* pEvent)
{
	const auto& paceOutput = getContext()->getPaceOutput();
	if (paceOutput.isOk() || paceOutput.isUndefined())
	{
		stopNfcScanIfNecessary();
	}
	else
	{
		const auto* volatileSettings = Env::getSingleton<VolatileSettings>();
		//: IOS The current session was interrupted because of a wrong password.
		stopNfcScanIfNecessary(volatileSettings->isUsedAsSDK() ? volatileSettings->getMessages().getSessionFailed() : tr("Access denied."));
	}

	AbstractState::onEntry(pEvent);
}
