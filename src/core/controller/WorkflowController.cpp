/*!
 * WorkflowController.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
