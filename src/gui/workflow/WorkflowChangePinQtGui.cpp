/*!
 * WorkflowChangePinQtGui.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WorkflowChangePinQtGui.h"

#include "AppQtMainWidget.h"
#include "PinSettingsWidget.h"
#include "generic/GuiUtils.h"
#include "states/StateChangePin.h"
#include "states/StateCleanUpReaderManager.h"
#include "states/StateEstablishPaceCan.h"
#include "states/StateEstablishPacePin.h"
#include "states/StateEstablishPacePuk.h"
#include "states/StateSelectReaderType.h"
#include "states/StateUpdateRetryCounter.h"
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
	if (mContext->getStatus().isError() && !mContext->isErrorReportedToUser())
	{
		if (!mContext->getStatus().isCancellationByUser())
		{
			activateStepUi(mErrorGui);
			mErrorGui->reportError();
		}
		mContext->setErrorReportedToUser();
	}

	if (mRetryCounterUpdated)
	{
		mRetryCounterUpdated = false;
		if (mContext->getLastPaceResult() != CardReturnCode::OK)
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

	if (AbstractState::isState<StateSelectReaderType>(pNextState))
	{
		mContext->setReaderType(ReaderManagerPlugInType::PCSC);
	}
	else if (AbstractState::isState<StateUpdateRetryCounter>(pNextState))
	{
		mRetryCounterUpdated = true;
	}
	else if (AbstractState::isState<StateEstablishPacePin>(pNextState))
	{
		mContext->setPin(mPinSettingsWidget->getPin());
	}
	else if (AbstractState::isState<StateEstablishPaceCan>(pNextState))
	{
		mContext->setCan(mPinSettingsWidget->getCan());
	}
	else if (AbstractState::isState<StateEstablishPacePuk>(pNextState))
	{
		mContext->setPuk(mPinSettingsWidget->getPuk());
	}
	else if (AbstractState::isState<StateChangePin>(pNextState))
	{
		mContext->setNewPin(mPinSettingsWidget->getNewPin());
	}
	else if (AbstractState::isState<StateCleanUpReaderManager>(pNextState) && mContext->getStatus().isNoError())
	{
		bool pinBlocked = (mContext->getCardConnection()->getReaderInfo().getRetryCounter() == 0);
		mPinSettingsWidget->setMode(pinBlocked ? PinSettingsWidget::Mode::AfterPinUnblock : PinSettingsWidget::Mode::AfterPinChange);
	}

	mContext->setStateApproved();
}
