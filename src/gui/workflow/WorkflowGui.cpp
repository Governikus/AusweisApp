/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WorkflowGui.h"

#include "generic/Page.h"
#include "step/StepGui.h"

using namespace governikus;

WorkflowGuiDelegate::WorkflowGuiDelegate()
{
}


WorkflowGui::WorkflowGui()
	: mDelegate(new WorkflowGuiDelegate)
{
}


WorkflowGui::~WorkflowGui()
{
}
