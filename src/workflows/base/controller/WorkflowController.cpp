/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "controller/WorkflowController.h"

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	#include "ReaderManager.h"
#endif

#include <QDebug>

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
