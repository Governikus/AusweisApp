/*!
 * WorkflowSelfInfoQtGui.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WorkflowSelfInfoQtGui.h"

#include "AppSettings.h"
#include "generic/GuiUtils.h"
#include "states/FinalState.h"
#include "states/StateDidAuthenticateEac1.h"
#include "states/StateDidAuthenticateEac2.h"
#include "states/StateEditAccessRights.h"
#include "states/StateEstablishPaceCan.h"
#include "states/StateEstablishPacePin.h"
#include "states/StateEstablishPacePuk.h"
#include "states/StateLoadTcTokenUrl.h"
#include "states/StateSelectPcscReader.h"
#include "states/StateTransmit.h"
#include "states/StateWriteHistory.h"
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
	, mCanEntered(false)
	, mAuthenticateStepsWidget(pParentWidget->findChild<AuthenticateStepsWidget*>())
	, mAdviseUserToRemoveCardGui(new StepAdviseUserToRemoveCardGui(mContext, mParentWidget))
	, mDidAuthenticateGui(new StepAuthenticationEac1Gui(mContext, mAuthenticateStepsWidget))
	, mChooseCardGui(new StepChooseCardGui(mContext, mAuthenticateStepsWidget))
	, mErrorGui(new StepErrorGui(mContext, mParentWidget))
	, mProcessingGui(new StepProcessingGui(mContext, mAuthenticateStepsWidget))
	, mShowSelfAuthenticationDataGui(new StepShowSelfAuthenticationDataGui(mContext, mAuthenticateStepsWidget))
{
	Q_ASSERT(mAuthenticateStepsWidget != nullptr);
	connect(mWidget, &WorkflowQtWidget::fireUserCancelled, this, &WorkflowGui::fireUserCancelled);
	connect(mWidget, &WorkflowQtWidget::forwardStep, this, &WorkflowSelfInfoQtGui::onForwardStep);
}


WorkflowSelfInfoQtGui::~WorkflowSelfInfoQtGui()
{
}


void WorkflowSelfInfoQtGui::activate()
{
	activateStepUi(mProcessingGui);
	mParentWidget->workflowActivated(WorkflowWidgetParent::SelfAuthentication, tr("Identify"));
	connect(mContext.data(), &SelfAuthenticationContext::fireCurrentStateChanged, this, &WorkflowSelfInfoQtGui::onCurrentStateChanged);
}


void WorkflowSelfInfoQtGui::deactivate()
{
	mParentWidget->workflowDeactivated();
}


bool WorkflowSelfInfoQtGui::verifyAbortWorkflow()
{
	// not really necessary to notify the user
	return true;
}


void WorkflowSelfInfoQtGui::onForwardStep()
{
	if (mStepGui)
	{
		mStepGui->forwardStep();
	}
}


void WorkflowSelfInfoQtGui::onCurrentStateChanged(const QString& pNewState)
{
	if (mContext->getStatus().isError() && !mContext->isErrorReportedToUser())
	{
		if (!mContext->getStatus().isCancellationByUser())
		{
			activateStepUi(mErrorGui);
			mErrorGui->reportError();
		}
		mContext->setErrorReportedToUser();
	}


	bool approveNewState = true;
	if (AbstractState::isState<StateLoadTcTokenUrl>(pNewState))
	{
		GeneralSettings& settings = AppSettings::getInstance().getGeneralSettings();
		approveNewState = !settings.isTransportPinReminder();
	}
	else if (AbstractState::isState<StateEditAccessRights>(pNewState))
	{
		approveNewState = false;
		activateStepUi(mDidAuthenticateGui);
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::EDIT_CHAT);
	}
	else if (AbstractState::isState<StateSelectPcscReader>(pNewState))
	{
		activateStepUi(mChooseCardGui);
	}
	else if (AbstractState::isState<StateEstablishPacePin>(pNewState) || AbstractState::isState<StateEstablishPaceCan>(pNewState))
	{
		if (AbstractState::isState<StateEstablishPaceCan>(pNewState))
		{
			approveNewState = !mContext->getCardConnection()->getReaderInfo().isBasicReader();
			mCanEntered = true;
		}
		else if (AbstractState::isState<StateEstablishPacePin>(pNewState))
		{
			// PIN entry after CAN entry is done without user interaction
			approveNewState = mCanEntered || !mContext->getCardConnection()->getReaderInfo().isBasicReader();
		}
		activateStepUi(mDidAuthenticateGui);
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::ENTER_PIN);
		if (mContext->getLastPaceResult() != CardReturnCode::OK)
		{
			mDidAuthenticateGui->incorrectPinError();
		}
	}
	else if (AbstractState::isState<StateEstablishPacePuk>(pNewState))
	{
		approveNewState = false;
		Q_EMIT mContext->fireCancelWorkflow();
		if (GuiUtils::showWrongPinBlockedDialog(mWidget))
		{
			mContext->setReaderName(QString());
			mParentWidget->switchToPinSettingsAfterWorkflow();
		}
	}
	else if (AbstractState::isState<StateDidAuthenticateEac1>(pNewState))
	{
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::AUTHENTICATING_ESERVICE);
	}
	else if (AbstractState::isState<StateDidAuthenticateEac2>(pNewState))
	{
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::AUTHENTICATING_CARD);
	}
	else if (AbstractState::isState<StateTransmit>(pNewState))
	{
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::READING_CARD_DATA);
	}
	else if (AbstractState::isState<StateWriteHistory>(pNewState) && isActive(mDidAuthenticateGui))
	{
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::FINISHED);
	}
	else if (AbstractState::isState<FinalState>(pNewState))
	{
		activateStepUi(mAdviseUserToRemoveCardGui);
		if (mContext->getStatus().isNoError())
		{
			approveNewState = false;
			activateStepUi(mShowSelfAuthenticationDataGui);
		}
	}
	mContext->setStateApproved(approveNewState);
}
