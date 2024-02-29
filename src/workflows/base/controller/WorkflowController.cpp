/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "controller/WorkflowController.h"

#include "states/AbstractState.h"


using namespace governikus;


WorkflowController::WorkflowController(const QSharedPointer<WorkflowContext>& pContext)
	: mStateMachine()
	, mContext(pContext)
{
	connect(&mStateMachine, &QStateMachine::finished, this, &WorkflowController::fireComplete, Qt::QueuedConnection);
}


void WorkflowController::forceStartStopScan()
{
	mStateMachine.setProperty(AbstractState::cFORCE_START_STOP_SCAN, true);
}


void WorkflowController::run()
{
	mStateMachine.start();
}
