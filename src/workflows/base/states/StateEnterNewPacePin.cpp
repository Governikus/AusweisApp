/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#include "StateEnterNewPacePin.h"


using namespace governikus;


StateEnterNewPacePin::StateEnterNewPacePin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setHandleNfcStop();
}


void StateEnterNewPacePin::run()
{
	Q_EMIT fireContinue();
}


void StateEnterNewPacePin::onEntry(QEvent* pEvent)
{
	stopNfcScanIfNecessary();

	AbstractState::onEntry(pEvent);
}
