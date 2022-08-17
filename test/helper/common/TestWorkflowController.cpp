/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "TestWorkflowController.h"

using namespace governikus;


TestWorkflowController::TestWorkflowController(const QSharedPointer<TestWorkflowContext>& pContext)
	: WorkflowController(pContext)
{
}
