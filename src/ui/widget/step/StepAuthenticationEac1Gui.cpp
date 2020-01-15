/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "StepAuthenticationEac1Gui.h"

#include "generic/GuiUtils.h"
#include "step/AuthenticateStepsWidget.h"

#include <QTimer>

using namespace governikus;

StepAuthenticationEac1Gui::StepAuthenticationEac1Gui(QSharedPointer<AuthContext> pContext,
		AuthenticateStepsWidget* pStepsWidget)
	: StepGui(pContext)
	, mContext(pContext)
	, mStepsWidget(pStepsWidget)
	, mWidget(nullptr)
	, mState(StepAuthenticationEac1Widget::State::EDIT_CHAT)
	, mPayAttentionToReaderMsgBox(new QMessageBox(pStepsWidget->window()))
	, mActive(false)
{
	mPayAttentionToReaderMsgBox->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
	mPayAttentionToReaderMsgBox->setText(tr("Please observe the display of your card reader."));
	mPayAttentionToReaderMsgBox->setIcon(QMessageBox::Information);
	mPayAttentionToReaderMsgBox->setStandardButtons(QMessageBox::NoButton);
}


StepAuthenticationEac1Gui::~StepAuthenticationEac1Gui()
{
}


void StepAuthenticationEac1Gui::activate()
{
	mActive = true;

	mWidget = mStepsWidget->getEac1Page();

	connect(mWidget, &StepAuthenticationEac1Widget::setForwardButtonState, getStepGuiDelegate(), &StepGuiDelegate::setForwardButtonState);
	connect(mWidget, &StepAuthenticationEac1Widget::setCancelButtonState, getStepGuiDelegate(), &StepGuiDelegate::setCancelButtonState);

	connect(mWidget, &StepAuthenticationEac1Widget::fireCanUpdated, this, &StepAuthenticationEac1Gui::onCanUpdated);
	connect(mWidget, &StepAuthenticationEac1Widget::firePinUpdated, this, &StepAuthenticationEac1Gui::onPinUpdated);

	connect(this, &StepAuthenticationEac1Gui::fireUiFinished, this, &StepAuthenticationEac1Gui::onUiFinished);

	mWidget->setContext(mContext);
	mWidget->setState(StepAuthenticationEac1Widget::State::INITIAL);
	mStepsWidget->setCurrentWidget(mWidget);
}


void StepAuthenticationEac1Gui::deactivate()
{
	mWidget->setContext(QSharedPointer<AuthContext>());

	disconnect(mWidget, &StepAuthenticationEac1Widget::setForwardButtonState, getStepGuiDelegate(), &StepGuiDelegate::setForwardButtonState);
	disconnect(mWidget, &StepAuthenticationEac1Widget::setCancelButtonState, getStepGuiDelegate(), &StepGuiDelegate::setCancelButtonState);

	disconnect(mWidget, &StepAuthenticationEac1Widget::fireCanUpdated, this, &StepAuthenticationEac1Gui::onCanUpdated);
	disconnect(mWidget, &StepAuthenticationEac1Widget::firePinUpdated, this, &StepAuthenticationEac1Gui::onPinUpdated);

	disconnect(this, &StepAuthenticationEac1Gui::fireUiFinished, this, &StepAuthenticationEac1Gui::onUiFinished);

	mActive = false;
}


bool StepAuthenticationEac1Gui::isActive() const
{
	return mActive;
}


void StepAuthenticationEac1Gui::setState(StepAuthenticationEac1Widget::State pState)
{
	mState = pState;

	mWidget->setState(pState);

	if (pState == StepAuthenticationEac1Widget::State::FINISHED)
	{
		forwardStep();
	}
}


void StepAuthenticationEac1Gui::incorrectPinError()
{
	GuiUtils::showPinCanPukErrorDialog(mContext->getLastPaceResult(), mContext->isCanAllowedMode(), mStepsWidget->window());
}


void StepAuthenticationEac1Gui::forwardStep()
{
	if (mState == StepAuthenticationEac1Widget::State::FINISHED)
	{
		Q_EMIT fireUiFinished();
		return;
	}

	mWidget->forwardStep();

	Q_EMIT fireUiFinished();
}


void StepAuthenticationEac1Gui::hidePayAttentionToReader()
{
	mPayAttentionToReaderMsgBox->reject();
}


void StepAuthenticationEac1Gui::onShowPayAttentionToReader()
{
	mPayAttentionToReaderMsgBox->open();
}


void StepAuthenticationEac1Gui::onPinUpdated(const QString& pPin)
{
	if (mContext->isCanAllowedMode())
	{
		mContext->setCan(pPin);
	}
	else
	{
		mContext->setPin(pPin);
	}
}


void StepAuthenticationEac1Gui::onCanUpdated(const QString& pCan)
{
	mContext->setCan(pCan);
}


void StepAuthenticationEac1Gui::onUiFinished()
{
	mContext->setStateApproved();
}
