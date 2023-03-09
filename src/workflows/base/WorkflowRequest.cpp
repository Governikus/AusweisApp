/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowRequest.h"

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<WorkflowRequest>>("QSharedPointer<WorkflowRequest>");
		})


WorkflowRequest::WorkflowRequest(const std::function<QSharedPointer<WorkflowController>(const QSharedPointer<WorkflowContext>&)>& pGeneratorController,
		const std::function<QSharedPointer<WorkflowContext>()>& pGeneratorContext,
		const BusyHandler& pBusyHandler)
	: mGeneratorController(pGeneratorController)
	, mGeneratorContext(pGeneratorContext)
	, mBusyHandler(pBusyHandler)
	, mController()
	, mContext(mGeneratorContext())
{
}


void WorkflowRequest::initialize()
{
	if (!isInitialized())
	{
		mController = mGeneratorController(mContext);
	}
}


bool WorkflowRequest::isInitialized() const
{
	return !mController.isNull();
}


Action WorkflowRequest::getAction() const
{
	return mContext->getAction();
}


QSharedPointer<WorkflowController> WorkflowRequest::getController() const
{
	return mController;
}


QSharedPointer<WorkflowContext> WorkflowRequest::getContext() const
{
	return mContext;
}


WorkflowControl WorkflowRequest::handleBusyWorkflow(const QSharedPointer<WorkflowRequest>& pActiveWorkflow, const QSharedPointer<WorkflowRequest>& pWaitingWorkflow)
{
	return mBusyHandler ? mBusyHandler(*this, pActiveWorkflow, pWaitingWorkflow) : WorkflowControl::UNHANDLED;
}
