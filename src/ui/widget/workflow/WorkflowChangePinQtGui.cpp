/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowChangePinQtGui.h"

#include "AppQtMainWidget.h"
#include "generic/GuiUtils.h"
#include "PinSettingsWidget.h"
#include "states/StateChangePin.h"
#include "states/StateCleanUpReaderManager.h"
#include "states/StateConnectCard.h"
#include "states/StateEnterNewPacePin.h"
#include "states/StateEnterPacePassword.h"
#include "states/StateEstablishPaceChannel.h"
#include "states/StateMaintainCardConnection.h"
#include "states/StateSelectReader.h"
#include "step/StepChooseCardGui.h"
#include "step/StepErrorGui.h"

using namespace governikus;

WorkflowChangePinQtGui::WorkflowChangePinQtGui(QSharedPointer<ChangePinContext> pContext, AppQtMainWidget* const pParentWidget)
	: GenericWorkflowGui(pContext, pParentWidget, nullptr)
	, mPinSettingsWidget(mParentWidget->findChild<PinSettingsWidget*>())
	, mChooseCardGui(new StepChooseCardGui(mContext, mPinSettingsWidget))
	, mErrorGui(new StepErrorGui(mContext, mParentWidget))
{
	Q_ASSERT(mPinSettingsWidget);
	connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &WorkflowChangePinQtGui::onStateChanged);
}


WorkflowChangePinQtGui::~WorkflowChangePinQtGui()
{
}


void WorkflowChangePinQtGui::activate()
{
	mParentWidget->workflowActivated(WorkflowWidgetParent::SettingsChangePin, QString());
	mPinSettingsWidget->setContext(mContext);
}


void WorkflowChangePinQtGui::deactivate()
{
	mPinSettingsWidget->setContext(QSharedPointer<ChangePinContext>());
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

	if (AbstractState::isState<StateSelectReader>(pNextState))
	{
		activateStepUi(mChooseCardGui);
	}
	else if (AbstractState::isState<StateConnectCard>(pNextState))
	{
		GenericWorkflowGui::deactivate();
	}
	else if (AbstractState::isState<StateEnterPacePassword>(pNextState))
	{
		const bool isBasicReader = mContext->getCardConnection()->getReaderInfo().isBasicReader();
		if (mContext->getEstablishPaceChannelType() == PacePasswordId::PACE_PUK)
		{
			mPinSettingsWidget->updatePinButton(isBasicReader && mPinSettingsWidget->getPuk().isEmpty());
		}
		else
		{
			mPinSettingsWidget->updatePinButton(isBasicReader && mPinSettingsWidget->getPin().isEmpty());
		}

		if (CardReturnCodeUtil::equalsWrongPacePassword(mContext->getLastPaceResult()))
		{
			if (isBasicReader)
			{
				mPinSettingsWidget->updatePasswordFields();
				mPinSettingsWidget->setPasswordFocus();
			}
			GuiUtils::showPinCanPukErrorDialog(mContext->getLastPaceResult(), mContext->isCanAllowedMode(), mPinSettingsWidget);
			if (isBasicReader)
			{
				return;
			}
		}

		if (mPinSettingsWidget->getPinButtonEnabled())
		{
			return;
		}
	}
	else if (AbstractState::isState<StateEstablishPaceChannel>(pNextState))
	{
		if (mContext->getCardConnection()->getReaderInfo().isBasicReader())
		{
			Q_ASSERT(!mPinSettingsWidget->getPin().isNull());
			Q_ASSERT(!mPinSettingsWidget->getCan().isNull());
			Q_ASSERT(!mPinSettingsWidget->getPuk().isNull());
			mContext->setPin(mPinSettingsWidget->getPin());
			mContext->setCan(mPinSettingsWidget->getCan());
			mContext->setPuk(mPinSettingsWidget->getPuk());
		}
	}
	else if (AbstractState::isState<StateChangePin>(pNextState))
	{
		if (mContext->getCardConnection()->getReaderInfo().isBasicReader())
		{
			Q_ASSERT(!mPinSettingsWidget->getNewPin().isNull());
			mContext->setNewPin(mPinSettingsWidget->getNewPin());
		}
	}
	else if (AbstractState::isState<StateCleanUpReaderManager>(pNextState) && mContext->getStatus().isNoError())
	{
		bool pinBlocked = false;
		if (mContext->getCardConnection())
		{
			pinBlocked = (mContext->getCardConnection()->getReaderInfo().getRetryCounter() == 0);
		}
		mPinSettingsWidget->setMode(pinBlocked ? PinSettingsWidget::Mode::AfterPinUnblock : PinSettingsWidget::Mode::AfterPinChange);
	}

	mContext->setStateApproved();
}
