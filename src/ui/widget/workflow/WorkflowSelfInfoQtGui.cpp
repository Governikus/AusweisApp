/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowSelfInfoQtGui.h"

#include "AppSettings.h"
#include "generic/GuiUtils.h"
#include "states/FinalState.h"
#include "states/StateCheckRefreshAddress.h"
#include "states/StateDidAuthenticateEac1.h"
#include "states/StateDidAuthenticateEac2.h"
#include "states/StateEditAccessRights.h"
#include "states/StateEnterPacePassword.h"
#include "states/StateLoadTcTokenUrl.h"
#include "states/StateMaintainCardConnection.h"
#include "states/StateSelectReader.h"
#include "states/StateTransmit.h"
#include "states/StateUnfortunateCardPosition.h"
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


WorkflowSelfInfoQtGui::WorkflowSelfInfoQtGui(const QSharedPointer<SelfAuthContext>& pContext, AppQtMainWidget* const pParentWidget)
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
	connect(mContext.data(), &SelfAuthContext::fireStateChanged, this, &WorkflowSelfInfoQtGui::onStateChanged);
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


void WorkflowSelfInfoQtGui::onStateChanged(const QString& pNewState)
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
		GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		approveNewState = !settings.isTransportPinReminder();
	}
	else if (AbstractState::isState<StateEditAccessRights>(pNewState))
	{
		approveNewState = false;
		activateStepUi(mDidAuthenticateGui);
		mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::EDIT_CHAT);
	}
	else if (AbstractState::isState<StateMaintainCardConnection>(pNewState))
	{
		mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::INITIAL);
		if (mContext->getLastPaceResult() != CardReturnCode::OK && !mContext->isPaceResultReportedToUser())
		{
			mContext->setPaceResultReportedToUser();
			mDidAuthenticateGui->incorrectPinError();
		}
	}
	else if (AbstractState::isState<StateSelectReader>(pNewState))
	{
		mContext->setStateApproved(true);
		activateStepUi(mChooseCardGui);
		return;
	}
	else if (AbstractState::isState<StateEnterPacePassword>(pNewState))
	{
		const PacePasswordId passwordId = mContext->getEstablishPaceChannelType();
		if (passwordId == PacePasswordId::PACE_PIN || passwordId == PacePasswordId::PACE_CAN)
		{
			if (passwordId == PacePasswordId::PACE_CAN)
			{
				approveNewState = !mContext->getCardConnection()->getReaderInfo().isBasicReader();
				mCanEntered = true;
			}
			else if (passwordId == PacePasswordId::PACE_PIN)
			{
				// PIN entry after CAN entry is done without user interaction
				approveNewState = mCanEntered || !mContext->getCardConnection()->getReaderInfo().isBasicReader();
			}
			activateStepUi(mDidAuthenticateGui);
			mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::ENTER_PIN);
		}
		else if (passwordId == PacePasswordId::PACE_PUK)
		{
			approveNewState = false;
			Q_EMIT mContext->fireCancelWorkflow();
			if (GuiUtils::showWrongPinBlockedDialog(mWidget))
			{
				mContext->setReaderName(QString());
				mParentWidget->switchToPinSettingsAfterWorkflow();
			}
		}
	}
	else if (AbstractState::isState<StateUnfortunateCardPosition>(pNewState))
	{
		approveNewState = false;
		activateStepUi(mChooseCardGui);
	}
	else if (AbstractState::isState<StateDidAuthenticateEac1>(pNewState))
	{
		activateStepUi(mDidAuthenticateGui);
		mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::AUTHENTICATING_ESERVICE);
	}
	else if (AbstractState::isState<StateDidAuthenticateEac2>(pNewState))
	{
		mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::AUTHENTICATING_CARD);
	}
	else if (AbstractState::isState<StateTransmit>(pNewState))
	{
		mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::READING_CARD_DATA);
	}
	else if (AbstractState::isState<StateCheckRefreshAddress>(pNewState) && mDidAuthenticateGui->isActive())
	{
		mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::REDIRECTING_BROWSER);
	}
	else if (AbstractState::isState<StateWriteHistory>(pNewState) && mDidAuthenticateGui->isActive())
	{
		mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::FINISHED);
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


void WorkflowSelfInfoQtGui::onCloseActiveDialogs()
{
	mAdviseUserToRemoveCardGui->closeActiveDialogs();
	mErrorGui->closeActiveDialogs();
}
