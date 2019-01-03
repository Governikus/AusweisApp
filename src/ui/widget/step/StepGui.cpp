/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StepGui.h"

#include "context/WorkflowContext.h"

using namespace governikus;

StepGuiDelegate::StepGuiDelegate()
	: QObject()
{
}


StepGui::StepGui(const QSharedPointer<WorkflowContext>& pContext)
	: mDelegate(new StepGuiDelegate)
{
	connect(this, &StepGui::fireCancelled, pContext.data(), &WorkflowContext::fireCancelWorkflow);
}


StepGui::~StepGui()
{
}


void StepGui::forwardStep()
{
	// can be implemented by subclasses
}
