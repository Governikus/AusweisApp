/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowGui.h"

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


void WorkflowGui::onCloseActiveDialogs()
{
}
