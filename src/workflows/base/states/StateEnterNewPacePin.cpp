/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterNewPacePin.h"


using namespace governikus;


StateEnterNewPacePin::StateEnterNewPacePin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setKeepCardConnectionAlive();
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
