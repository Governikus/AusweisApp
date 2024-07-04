/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "controller/WorkflowController.h"


using namespace governikus;


WorkflowController::WorkflowController(const QSharedPointer<WorkflowContext>& pContext)
	: mStateMachine()
	, mContext(pContext)
{
	connect(&mStateMachine, &QStateMachine::finished, this, &WorkflowController::fireComplete, Qt::QueuedConnection);
}


void WorkflowController::run()
{
	mStateMachine.start();
}
