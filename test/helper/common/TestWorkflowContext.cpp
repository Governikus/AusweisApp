/*!
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

#include "TestWorkflowContext.h"


using namespace governikus;


TestWorkflowContext::TestWorkflowContext(bool pCanAllowed)
	: WorkflowContext()
	, mCanAllowed(pCanAllowed)
{
}


bool TestWorkflowContext::isCanAllowedMode() const
{
	return mCanAllowed;
}


void TestWorkflowContext::setCanAllowedMode(bool pCanAllowed)
{
	mCanAllowed = pCanAllowed;
	Q_EMIT fireCanAllowedModeChanged();
}
