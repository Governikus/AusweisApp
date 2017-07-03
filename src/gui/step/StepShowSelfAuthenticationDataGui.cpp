/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StepShowSelfAuthenticationDataGui.h"

#include "step/AuthenticateStepsWidget.h"
#include "step/SelfInfoWidget.h"

using namespace governikus;

StepShowSelfAuthenticationDataGui::StepShowSelfAuthenticationDataGui(QSharedPointer<SelfAuthenticationContext> pContext,
		AuthenticateStepsWidget* pStepsWidget)
	: StepGui(pContext)
	, mContext(pContext)
	, mStepsWidget(pStepsWidget)
{
}


StepShowSelfAuthenticationDataGui::~StepShowSelfAuthenticationDataGui()
{
}


void StepShowSelfAuthenticationDataGui::activate()
{
	SelfInfoWidget* selfInfoWidget = mStepsWidget->getSelfInfoPage();
	selfInfoWidget->setInfo(mContext->getSelfAuthenticationData().data());
	mStepsWidget->setCurrentWidget(selfInfoWidget);

	setForwardButtonState(ButtonState::FOCUSSED, tr("Close"));
	setCancelButtonState(ButtonState::HIDDEN);
}


void StepShowSelfAuthenticationDataGui::deactivate()
{
}


void StepShowSelfAuthenticationDataGui::forwardStep()
{
	mContext->setStateApproved();
}
