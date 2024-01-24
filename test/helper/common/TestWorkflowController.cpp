/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "TestWorkflowController.h"

using namespace governikus;


TestWorkflowController::TestWorkflowController(const QSharedPointer<WorkflowContext>& pContext)
	: WorkflowController(pContext)
{
}
