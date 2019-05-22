/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StepShowSelfAuthenticationDataGui.h"

#include "step/AuthenticateStepsWidget.h"
#include "step/SelfInfoWidget.h"

using namespace governikus;

StepShowSelfAuthenticationDataGui::StepShowSelfAuthenticationDataGui(QSharedPointer<SelfAuthContext> pContext,
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
	selfInfoWidget->setInfo(mContext->getSelfAuthenticationData());
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
