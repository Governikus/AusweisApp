/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
#include "states/StateUnfortunateCardPosition.h"
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
	connect(mContext.data(), &ChangePinContext::firePaceResultUpdated, this, &WorkflowChangePinQtGui::onPaceResultUpdated);
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


void WorkflowChangePinQtGui::onStateChanged(const QString& pNewState)
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

	if (AbstractState::isState<StateSelectReader>(pNewState))
	{
		activateStepUi(mChooseCardGui);
	}
	else if (AbstractState::isState<StateConnectCard>(pNewState))
	{
		GenericWorkflowGui::deactivate();
	}
	else if (AbstractState::isState<StateEnterPacePassword>(pNewState))
	{
		const bool isBasicReader = mContext->getCardConnection()->getReaderInfo().isBasicReader();
		bool stateAccepted;
		if (mContext->getEstablishPaceChannelType() == PacePasswordId::PACE_PUK)
		{
			stateAccepted = !(isBasicReader && mPinSettingsWidget->getPuk().isEmpty());
		}
		else
		{
			stateAccepted = !(isBasicReader && mPinSettingsWidget->getPin().isEmpty());
		}

		mPinSettingsWidget->updatePinButton(false);

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

		if (!stateAccepted)
		{
			if (isBasicReader)
			{
				mPinSettingsWidget->updatePasswordFields();
				mPinSettingsWidget->setPasswordFocus();
			}
			return;
		}
	}
	else if (AbstractState::isState<StateUnfortunateCardPosition>(pNewState))
	{
		activateStepUi(mChooseCardGui);
		return;
	}
	else if (AbstractState::isState<StateEstablishPaceChannel>(pNewState))
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
	else if (AbstractState::isState<StateChangePin>(pNewState))
	{
		if (mContext->getCardConnection()->getReaderInfo().isBasicReader())
		{
			Q_ASSERT(!mPinSettingsWidget->getNewPin().isNull());
			mContext->setNewPin(mPinSettingsWidget->getNewPin());
		}
	}
	else if (AbstractState::isState<StateCleanUpReaderManager>(pNewState) && mContext->getStatus().isNoError())
	{
		mPinSettingsWidget->setMode(PinSettingsWidget::Mode::AfterPinChange);
	}
	mContext->setStateApproved();
}


void WorkflowChangePinQtGui::onPaceResultUpdated()
{
	if (mContext->getLastPaceResult() == CardReturnCode::OK_PUK)
	{
		GuiUtils::showPinUnlockedDialog(mPinSettingsWidget);
	}
}
