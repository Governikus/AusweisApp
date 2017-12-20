/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowAuthenticateQtGui.h"

#include "AppSettings.h"
#include "generic/GuiUtils.h"
#include "states/FinalState.h"
#include "states/StateCheckRefreshAddress.h"
#include "states/StateDidAuthenticateEac1.h"
#include "states/StateDidAuthenticateEac2.h"
#include "states/StateEditAccessRights.h"
#include "states/StateEstablishPaceCan.h"
#include "states/StateEstablishPacePin.h"
#include "states/StateEstablishPacePuk.h"
#include "states/StateProcessing.h"
#include "states/StateSelectReader.h"
#include "states/StateTransmit.h"
#include "states/StateWriteHistory.h"
#include "step/AuthenticateStepsWidget.h"
#include "step/StepAdviseUserToRemoveCardGui.h"
#include "step/StepAuthenticationDoneGui.h"
#include "step/StepAuthenticationEac1Gui.h"
#include "step/StepChooseCardGui.h"
#include "step/StepErrorGui.h"
#include "step/StepProcessingGui.h"
#include "workflow/WorkflowQtWidget.h"


using namespace governikus;


WorkflowAuthenticateQtGui::WorkflowAuthenticateQtGui(const QSharedPointer<AuthContext>& pContext, AppQtMainWidget* const pParentWidget)
	: GenericWorkflowGui(pContext, pParentWidget, pParentWidget->getAuthenticationWorkflowWidget())
	, mCanEntered(false)
	, mAuthenticateStepsWidget(mParentWidget->findChild<AuthenticateStepsWidget*>())
	, mAdviseUserToRemoveCardGui(new StepAdviseUserToRemoveCardGui(mContext, mParentWidget))
	, mDidAuthenticateGui(new StepAuthenticationEac1Gui(mContext, mAuthenticateStepsWidget))
	, mChooseCardGui(new StepChooseCardGui(mContext, mAuthenticateStepsWidget))
	, mErrorGui(new StepErrorGui(mContext, mParentWidget))
	, mProcessingGui(new StepProcessingGui(mContext, mAuthenticateStepsWidget))
	, mAuthenticationDoneGui(new StepAuthenticationDoneGui(mContext))
{
	Q_ASSERT(mAuthenticateStepsWidget != nullptr);
	connect(mWidget, &WorkflowQtWidget::fireUserCancelled, this, &WorkflowGui::fireUserCancelled);
	connect(mWidget, &WorkflowQtWidget::forwardStep, this, &WorkflowAuthenticateQtGui::onForwardStep);
	connect(mChooseCardGui.data(), &StepChooseCardGui::fireSwitchToReaderSettingsRequested, this, &WorkflowGui::fireSwitchToReaderSettingsRequested);
}


WorkflowAuthenticateQtGui::~WorkflowAuthenticateQtGui()
{
}


void WorkflowAuthenticateQtGui::activate()
{
	activateStepUi(mProcessingGui);
	mParentWidget->workflowActivated(WorkflowWidgetParent::Authentication, tr("Identify"));
	connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &WorkflowAuthenticateQtGui::onStateChanged);
}


void WorkflowAuthenticateQtGui::deactivate()
{
	mParentWidget->workflowDeactivated();
}


bool WorkflowAuthenticateQtGui::verifyAbortWorkflow()
{
	QMessageBox msgBox(mParentWidget);
	msgBox.setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Cancel"));
	msgBox.setWindowModality(Qt::WindowModal);
	msgBox.setText(tr("<b>Do you really want to cancel?</b>"));
	msgBox.setInformativeText(tr("You can as well identity later by calling the service provider's Internet page"
								 " again."));
	msgBox.setIconPixmap(QIcon(QStringLiteral(":/images/npa.svg")).pixmap(32, 32));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setWindowFlags(msgBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	return msgBox.exec() == QMessageBox::Yes;
}


void WorkflowAuthenticateQtGui::onForwardStep()
{
	if (mStepGui != nullptr)
	{
		mStepGui->forwardStep();
	}
}


void WorkflowAuthenticateQtGui::onStateChanged(const QString& pNewState)
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
	if (AbstractState::isState<StateProcessing>(pNewState))
	{
		activateStepUi(mProcessingGui);

		GeneralSettings& settings = AppSettings::getInstance().getGeneralSettings();
		approveNewState = !settings.isTransportPinReminder();
	}
	else if (AbstractState::isState<StateEditAccessRights>(pNewState))
	{
		approveNewState = false;
		activateStepUi(mDidAuthenticateGui);
		mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::EDIT_CHAT);
	}
	else if (AbstractState::isState<StateSelectReader>(pNewState))
	{
		mContext->setStateApproved(true);
		activateStepUi(mChooseCardGui);
		return;
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
		mDidAuthenticateGui->setState(StepAuthenticationEac1Widget::State::ENTER_PIN);
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
		activateStepUi(mAuthenticationDoneGui);
	}
	mContext->setStateApproved(approveNewState);
}
