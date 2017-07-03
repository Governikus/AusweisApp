/*!
 * WorkflowAuthenticateQtGui.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WorkflowAuthenticateQtGui.h"

#include "AppSettings.h"
#include "generic/GuiUtils.h"
#include "step/AuthenticateStepsWidget.h"
#include "step/StepAdviseUserToRemoveCardGui.h"
#include "step/StepAuthenticationDoneGui.h"
#include "step/StepAuthenticationEac1Gui.h"
#include "step/StepChooseCardGui.h"
#include "step/StepErrorGui.h"
#include "step/StepProcessingGui.h"


using namespace governikus;


WorkflowAuthenticateQtGui::WorkflowAuthenticateQtGui(const QSharedPointer<AuthContext>& pContext, AppQtMainWidget* const pParentWidget)
	: GenericWorkflowGui(pContext, pParentWidget, pParentWidget->getAuthenticationWorkflowWidget())
	, mRetryCounterUpdated(false)
	, mAuthenticateStepsWidget(mParentWidget->findChild<AuthenticateStepsWidget*>())
	, mAdviseUserToRemoveCardGui(new StepAdviseUserToRemoveCardGui(mContext, mParentWidget))
	, mAuthenticationDoneGui(new StepAuthenticationDoneGui(mContext))
	, mDidAuthenticateGui(new StepAuthenticationEac1Gui(mContext, mAuthenticateStepsWidget))
	, mChooseCardGui(new StepChooseCardGui(mContext, mAuthenticateStepsWidget))
	, mErrorGui(new StepErrorGui(mContext, mParentWidget))
	, mProcessingGui(new StepProcessingGui(mContext, mAuthenticateStepsWidget))
{
	Q_ASSERT(mAuthenticateStepsWidget != nullptr);

	GenericWorkflowGui<AuthContext>::activateStepUi(mProcessingGui);

	connect(mWidget, &WorkflowQtWidget::fireUserCancelled, this, &WorkflowGui::fireUserCancelled);
	connect(mWidget, &WorkflowQtWidget::forwardStep, this, &WorkflowAuthenticateQtGui::nextStep);
}


WorkflowAuthenticateQtGui::~WorkflowAuthenticateQtGui()
{
}


void WorkflowAuthenticateQtGui::activate()
{
	mParentWidget->workflowActivated(WorkflowWidgetParent::Authentication, tr("Identify"));
	connect(mContext.data(), &WorkflowContext::fireCurrentStateChanged, this, &WorkflowAuthenticateQtGui::onStateChanged);
}


void WorkflowAuthenticateQtGui::deactivate()
{
	mParentWidget->workflowDeactivated();
}


bool WorkflowAuthenticateQtGui::verifyAbortWorkflow()
{
	QMessageBox msgBox(mParentWidget);
	msgBox.setWindowTitle(QCoreApplication::applicationName() + " - " + tr("Cancel"));
	msgBox.setWindowModality(Qt::WindowModal);
	msgBox.setText(tr("<b>Do you really want to cancel?</b>"));
	msgBox.setInformativeText(tr("You can as well identity later by calling the service provider's Internet page"
								 " again."));
	QPixmap pixmap(QStringLiteral(":images/autentapp2.iconset/icon_32x32.png"));
	msgBox.setIconPixmap(pixmap);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setWindowFlags(msgBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	return msgBox.exec() == QMessageBox::Yes;
}


void WorkflowAuthenticateQtGui::handleBlockedPin()
{
	mContext->fireCancelWorkflow();
	if (GuiUtils::showWrongPinBlockedDialog(mWidget))
	{
		mParentWidget->switchToPinSettingsAfterWorkflow();
		mContext->setReaderName(QString());
	}
}


void WorkflowAuthenticateQtGui::nextStep()
{
	if (mStepGui != nullptr)
	{
		mStepGui->forwardStep();
	}
}


void WorkflowAuthenticateQtGui::onStateChanged(const QString& pNewState)
{
	if (!mContext->getResult().isOk() && !mContext->isErrorReportedToUser())
	{
		if (mContext->getResult().getMinor() != Result::Minor::SAL_Cancellation_by_User)
		{
			activateStepUi(mErrorGui);
			mErrorGui->reportError();
		}
		mContext->setErrorReportedToUser();
	}

	bool approveNewState = true;

	if (pNewState == QLatin1String("StateProcessing"))
	{
		activateStepUi(mProcessingGui);

		GeneralSettings& settings = AppSettings::getInstance().getGeneralSettings();
		approveNewState = !settings.isTransportPinReminder();
	}
	else if (pNewState == QLatin1String("StateSelectPcscReader"))
	{
		activateStepUi(mChooseCardGui);
	}
	else if (pNewState == QLatin1String("StateUpdateRetryCounter"))
	{
		mRetryCounterUpdated = true;
	}
	else if (mRetryCounterUpdated)
	{
		mRetryCounterUpdated = false;
		approveNewState = false;
		if (mContext->isPinBlocked())
		{
			activateStepUi(mProcessingGui);
			handleBlockedPin();
		}
		else if (mContext->getLastPaceResult() != ReturnCode::OK)
		{
			mDidAuthenticateGui->incorrectPinError();
		}
		else
		{
			activateStepUi(mDidAuthenticateGui);
			mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::EDIT_CHAT);
		}
	}
	else if (pNewState == QLatin1String("StateDidAuthenticateEac1"))
	{
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::AUTHENTICATING_ESERVICE);
	}
	else if (pNewState == QLatin1String("StateDidAuthenticateEac2"))
	{
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::AUTHENTICATING_CARD);
	}
	else if (pNewState == QLatin1String("StateTransmit"))
	{
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::READING_CARD_DATA);
	}
	else if (pNewState == QLatin1String("StateWriteHistory") && isActive(mDidAuthenticateGui))
	{
		mDidAuthenticateGui->setState(StepDidAuthenticateEac1Ui::State::FINISHED);
	}
	else if (pNewState == QLatin1String("FinalState"))
	{
		activateStepUi(mAdviseUserToRemoveCardGui);
		activateStepUi(mAuthenticationDoneGui);
	}


	mContext->setStateApproved(approveNewState);
}
