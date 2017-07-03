/*!
 * WorkflowChangePinQtGui.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WorkflowChangePinQtGui.h"

#include "AppQtMainWidget.h"
#include "PinSettingsWidget.h"
#include "generic/GuiUtils.h"
#include "step/StepChooseCardGui.h"
#include "step/StepErrorGui.h"

using namespace governikus;

WorkflowChangePinQtGui::WorkflowChangePinQtGui(QSharedPointer<ChangePinContext> pContext, AppQtMainWidget* const pParentWidget)
	: GenericWorkflowGui(pContext, pParentWidget, nullptr)
	, mRetryCounterUpdated(false)
	, mPinSettingsWidget(mParentWidget->findChild<PinSettingsWidget*>())
	, mErrorGui(new StepErrorGui(mContext, mParentWidget))
{
	Q_ASSERT(mPinSettingsWidget);
	connect(mContext.data(), &WorkflowContext::fireCurrentStateChanged, this, &WorkflowChangePinQtGui::onStateChanged);
}


WorkflowChangePinQtGui::~WorkflowChangePinQtGui()
{
}


void WorkflowChangePinQtGui::activate()
{
	mParentWidget->workflowActivated(WorkflowWidgetParent::SettingsChangePin, QString());
	mPinSettingsWidget->setInProgress(true);
}


void WorkflowChangePinQtGui::deactivate()
{
	mPinSettingsWidget->setInProgress(false);
	mParentWidget->workflowDeactivated();
}


bool WorkflowChangePinQtGui::verifyAbortWorkflow()
{
	// not really necessary to notify the user
	return true;
}


void WorkflowChangePinQtGui::onStateChanged(const QString& pNextState)
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

	if (mRetryCounterUpdated)
	{
		mRetryCounterUpdated = false;
		if (mContext->getLastPaceResult() != ReturnCode::OK)
		{
			auto newRetryCounter = mContext->getCardConnection()->getReaderInfo().getRetryCounter();
			GuiUtils::showPinCanPukErrorDialog(mContext->getLastPaceResult(), newRetryCounter, mPinSettingsWidget);

			/*
			 * In the desktop version we cancel the workflow after a wrong user input.
			 * If the user want's to try again, the workflow must be started again.
			 */
			Q_EMIT mContext->fireCancelWorkflow();
			return;
		}
	}

	if (pNextState == QLatin1String("StateSelectReaderType"))
	{
		mContext->setReaderType(ReaderManagerPlugInType::PCSC);
	}
	else if (pNextState == QLatin1String("StateUpdateRetryCounter"))
	{
		mRetryCounterUpdated = true;
	}
	else if (pNextState == QLatin1String("StateEstablishPacePin"))
	{
		mContext->setPin(mPinSettingsWidget->getPin());
	}
	else if (pNextState == QLatin1String("StateEstablishPaceCan"))
	{
		mContext->setCan(mPinSettingsWidget->getCan());
	}
	else if (pNextState == QLatin1String("StateEstablishPacePuk"))
	{
		mContext->setPuk(mPinSettingsWidget->getPuk());
	}
	else if (pNextState == QLatin1String("StateChangePin"))
	{
		mContext->setNewPin(mPinSettingsWidget->getNewPin());
	}
	else if (pNextState == QLatin1String("StateCleanUpReaderManager") && mContext->getResult().isOk())
	{
		bool pinBlocked = (mContext->getCardConnection()->getReaderInfo().getRetryCounter() == 0);
		mPinSettingsWidget->setMode(pinBlocked ? PinSettingsWidget::Mode::AfterPinUnblock : PinSettingsWidget::Mode::AfterPinChange);
	}

	mContext->setStateApproved();
}
