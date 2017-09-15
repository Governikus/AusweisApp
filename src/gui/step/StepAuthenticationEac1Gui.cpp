/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "generic/GuiUtils.h"
#include "step/AuthenticateStepsWidget.h"
#include "step/StepAuthenticationEac1Widget.h"
#include "StepAuthenticationEac1Gui.h"

#include <QTimer>

using namespace governikus;

StepAuthenticationEac1Gui::StepAuthenticationEac1Gui(QSharedPointer<AuthContext> pContext,
		AuthenticateStepsWidget* pStepsWidget)
	: StepGui(pContext)
	, mContext(pContext)
	, mStepsWidget(pStepsWidget)
	, mWidget(nullptr)
	, mState(StepDidAuthenticateEac1Ui::State::EDIT_CHAT)
	, mPayAttentionToReaderMsgBox(new QMessageBox(pStepsWidget->window()))
{
	mPayAttentionToReaderMsgBox->setWindowTitle(QCoreApplication::applicationName() + " - " + tr("Information"));
	mPayAttentionToReaderMsgBox->setText(tr("Please observe the display of your card reader."));
	mPayAttentionToReaderMsgBox->setIcon(QMessageBox::Information);
	mPayAttentionToReaderMsgBox->setStandardButtons(QMessageBox::NoButton);
}


StepAuthenticationEac1Gui::~StepAuthenticationEac1Gui()
{
}


void StepAuthenticationEac1Gui::activate()
{
	mWidget = mStepsWidget->getEac1Page();

	connect(mWidget, &StepAuthenticationEac1Widget::setForwardButtonState, getStepGuiDelegate(), &StepGuiDelegate::setForwardButtonState);
	connect(mWidget, &StepAuthenticationEac1Widget::setCancelButtonState, getStepGuiDelegate(), &StepGuiDelegate::setCancelButtonState);

	connect(mWidget, &StepAuthenticationEac1Widget::fireCanUpdated, this, &StepAuthenticationEac1Gui::onCanUpdated);
	connect(mWidget, &StepAuthenticationEac1Widget::firePinUpdated, this, &StepAuthenticationEac1Gui::onPinUpdated);

	connect(this, &StepAuthenticationEac1Gui::fireUiFinished, this, &StepAuthenticationEac1Gui::onUiFinished);

	mWidget->setContext(mContext);
	mWidget->setState(StepDidAuthenticateEac1Ui::State::INITIAL);
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
}


void StepAuthenticationEac1Gui::setState(StepDidAuthenticateEac1Ui::State pState)
{
	mState = pState;

	mWidget->setState(pState);

	if (pState == StepDidAuthenticateEac1Ui::State::FINISHED)
	{
		forwardStep();
	}
}


void StepAuthenticationEac1Gui::incorrectPinError()
{
	mWidget->updateButtonsAndPinWidget();

	GuiUtils::showPinCanPukErrorDialog(mContext->getLastPaceResult(), mContext->getCardConnection()->getReaderInfo().getRetryCounter(), mStepsWidget->window());
}


void StepAuthenticationEac1Gui::forwardStep()
{
	if (mState == StepDidAuthenticateEac1Ui::State::FINISHED)
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
	mPin = pPin;
}


void StepAuthenticationEac1Gui::onCanUpdated(const QString& pCan)
{
	mCan = pCan;
}


void StepAuthenticationEac1Gui::onUiFinished()
{
	mContext->setCan(mCan);
	mContext->setPin(mPin);
	mContext->setStateApproved();
}
