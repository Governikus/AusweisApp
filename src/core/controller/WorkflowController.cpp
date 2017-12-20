/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "controller/WorkflowController.h"

#include <QDebug>

using namespace governikus;

WorkflowController::WorkflowController(const QSharedPointer<WorkflowContext>& pContext)
	: mStateMachine()
	, mContext(pContext)
{
	connect(&mStateMachine, &QStateMachine::finished, this, &WorkflowController::fireComplete, Qt::QueuedConnection);
}


WorkflowController::~WorkflowController()
{
}


void WorkflowController::run()
{
	mStateMachine.start();
}
