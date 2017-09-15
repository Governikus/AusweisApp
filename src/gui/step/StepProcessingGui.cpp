/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "generic/BusyOverlayContainer.h"
#include "step/AuthenticateStepsWidget.h"
#include "StepProcessingGui.h"

using namespace governikus;

StepProcessingGui::StepProcessingGui(const QSharedPointer<WorkflowContext>& pContext, AuthenticateStepsWidget* pStepsWidget)
	: StepGui(pContext)
	, mStepsWidget(pStepsWidget)
{
}


StepProcessingGui::~StepProcessingGui()
{
}


void StepProcessingGui::activate()
{
	setCancelButtonState(ButtonState::ENABLED);
	setForwardButtonState(ButtonState::HIDDEN);

	mStepsWidget->setCurrentWidget(mStepsWidget->getProcessingPage());
	mStepsWidget->getProcessingPage()->startAnimation();
}


void StepProcessingGui::deactivate()
{
	mStepsWidget->getProcessingPage()->stopAnimation();
}
