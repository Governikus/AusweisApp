/*!
 * WorkflowSelfInfoQtGui.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WorkflowSelfInfoQtGui.h"

#include "AppQtMainWidget.h"
#include "AppSettings.h"
#include "generic/GuiUtils.h"
#include "step/AuthenticateStepsWidget.h"
#include "step/StepAdviseUserToRemoveCardGui.h"
#include "step/StepAuthenticationEac1Gui.h"
#include "step/StepChooseCardGui.h"
#include "step/StepErrorGui.h"
#include "step/StepProcessingGui.h"
#include "step/StepShowSelfAuthenticationDataGui.h"
#include "workflow/WorkflowQtWidget.h"

using namespace governikus;

WorkflowSelfInfoQtGui::WorkflowSelfInfoQtGui(const QSharedPointer<SelfAuthenticationContext>& pContext, AppQtMainWidget* const pParentWidget)
	: GenericWorkflowGui(pContext, pParentWidget, pParentWidget->getAuthenticationWorkflowWidget())
	, mRetryCounterUpdated(false)
	, mAuthenticateStepsWidget(pParentWidget->findChild<AuthenticateStepsWidget*>())
	, mStepAdviseUserToRemoveCardGui(new StepAdviseUserToRemoveCardGui(mContext, mParentWidget))
	, mStepChooseCardGui(new StepChooseCardGui(mContext, mAuthenticateStepsWidget))
	, mStepAuthenticationEac1Gui(new StepAuthenticationEac1Gui(mContext, mAuthenticateStepsWidget))
	, mStepErrorGui(new StepErrorGui(mContext, mParentWidget))
	, mStepProcessingGui(new StepProcessingGui(mContext, mAuthenticateStepsWidget))
	, mStepShowSelfAuthenticationDataGui(new StepShowSelfAuthenticationDataGui(mContext, mAuthenticateStepsWidget))
{
	Q_ASSERT(mAuthenticateStepsWidget != nullptr);

	connect(mWidget, &WorkflowQtWidget::fireUserCancelled, this, &WorkflowGui::fireUserCancelled);
	connect(mWidget, &WorkflowQtWidget::forwardStep, this, &WorkflowSelfInfoQtGui::onForwardStep);

	connect(mContext.data(), &SelfAuthenticationContext::fireCurrentStateChanged, this, &WorkflowSelfInfoQtGui::onCurrentStateChanged);
}


WorkflowSelfInfoQtGui::~WorkflowSelfInfoQtGui()
{
}


void WorkflowSelfInfoQtGui::activate()
{
	mParentWidget->workflowActivated(WorkflowWidgetParent::SelfAuthentication, tr("Identify"));
	activateStepUi(mStepProcessingGui);
}


void WorkflowSelfInfoQtGui::deactivate()
{
	WorkflowGui::deactivate();
	mParentWidget->workflowDeactivated();
}


bool WorkflowSelfInfoQtGui::verifyAbortWorkflow()
{
	// not really necessary to notify the user
	return true;
}


void WorkflowSelfInfoQtGui::onCurrentStateChanged(const QString& pNewState)
{
	if (!mContext->getResult().isOk() && !mContext->isErrorReportedToUser())
	{
		if (mContext->getResult().getMinor() != Result::Minor::SAL_Cancellation_by_User)
		{
			activateStepUi(mStepErrorGui);
			mStepErrorGui->reportError();
		}
		mContext->setErrorReportedToUser();
	}

	bool approveNewState = true;

	if (pNewState == QLatin1String("StateLoadTcTokenUrl"))
	{
		GeneralSettings& settings = AppSettings::getInstance().getGeneralSettings();
		approveNewState = !settings.isTransportPinReminder();
	}
	else if (pNewState == QLatin1String("StateSelectPcscReader"))
	{
		activateStepUi(mStepChooseCardGui);
	}
	else if (pNewState == QLatin1String("StateUpdateRetryCounter"))
	{
		mRetryCounterUpdated = true;
	}
	else if (mRetryCounterUpdated)
	{
		mRetryCounterUpdated = false;
		approveNewState = false;
		if (!showPinBlockageDialog())
		{
			if (mContext->getLastPaceResult() != ReturnCode::OK)
			{
				mStepAuthenticationEac1Gui->incorrectPinError();
			}
			else
			{
				activateStepUi(mStepAuthenticationEac1Gui);
				mStepAuthenticationEac1Gui->setState(StepDidAuthenticateEac1Ui::State::EDIT_CHAT);
			}
		}
	}
	else if (pNewState == QLatin1String("StateDidAuthenticateEac1"))
	{
		mStepAuthenticationEac1Gui->setState(StepDidAuthenticateEac1Ui::State::AUTHENTICATING_ESERVICE);
	}
	else if (pNewState == QLatin1String("StateDidAuthenticateEac2"))
	{
		mStepAuthenticationEac1Gui->setState(StepDidAuthenticateEac1Ui::State::AUTHENTICATING_CARD);
	}
	else if (pNewState == QLatin1String("StateTransmit"))
	{
		mStepAuthenticationEac1Gui->setState(StepDidAuthenticateEac1Ui::State::READING_CARD_DATA);
	}
	else if (pNewState == QLatin1String("StateWriteHistory") && isActive(mStepAuthenticationEac1Gui))
	{
		mStepAuthenticationEac1Gui->setState(StepDidAuthenticateEac1Ui::State::FINISHED);
	}
	else if (pNewState == QLatin1String("FinalState"))
	{
		activateStepUi(mStepAdviseUserToRemoveCardGui);
		if (mContext->getResult().isOk())
		{
			approveNewState = false;
			activateStepUi(mStepShowSelfAuthenticationDataGui);
		}
	}

	mContext->setStateApproved(approveNewState);
}


void WorkflowSelfInfoQtGui::onForwardStep()
{
	if (mStepGui)
	{
		mStepGui->forwardStep();
	}
}


bool WorkflowSelfInfoQtGui::showPinBlockageDialog()
{
	if (mContext->isPinBlocked())
	{
		activateStepUi(mStepProcessingGui);
		mContext->fireCancelWorkflow();

		if (GuiUtils::showWrongPinBlockedDialog(mWidget))
		{
			mContext->setReaderName(QString());
			mParentWidget->switchToPinSettingsAfterWorkflow();
		}

		return true;
	}

	return false;
}
