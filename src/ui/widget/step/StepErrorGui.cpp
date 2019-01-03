/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StepErrorGui.h"

#include "AppQtMainWidget.h"
#include "generic/GuiUtils.h"
#include "LogFileSaveDialog.h"

#include <QApplication>
#include <QLoggingCategory>
#include <QTimer>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


StepErrorGui::StepErrorGui(QSharedPointer<WorkflowContext> pContext, AppQtMainWidget* const pMainWidget)
	: StepGui(pContext)
	, mContext(pContext)
	, mMainWidget(pMainWidget)
	, mMessageBox()
{
}


StepErrorGui::~StepErrorGui()
{
}


void StepErrorGui::reportError()
{
	// Do not close the window automatically in case of errors when the workflow is done.
	mMainWidget->setHideWindowAfterWorkflow(false);

	if (mContext->getStatus().is(GlobalStatus::Code::Paos_Error_SAL_Invalid_Key) && !mContext->getCardConnection()->getReaderInfo().getCardInfo().isPinDeactivated())
	{
		if (GuiUtils::showWrongPinBlockedDialog(mMainWidget))
		{
			mMainWidget->switchToPinSettingsAfterWorkflow();
			Q_EMIT switchedToPinSettings();
		}
		else
		{
			Q_EMIT fireUiFinished();
		}
		return;
	}

	QString message = mContext->getStatus().toErrorDescription(true);
	if (message.isEmpty())
	{
		qCCritical(gui) << "No error message determined:" << mContext->getStatus();
		message = tr("Sorry, that should not have happened! Please contact the support team.");
		Q_ASSERT(!message.isEmpty());
	}

	// If workflow has been killed, we do not show anything.
	if (mContext->isWorkflowKilled())
	{
		Q_EMIT fireUiFinished();

		return;
	}

	if (mMessageBox.isNull())
	{
		mMessageBox = QPointer<QMessageBox>(new QMessageBox(mMainWidget));

		QPushButton* saveLogButton = nullptr;
		if (mContext->getStatus().is(GlobalStatus::Code::Card_Unexpected_Transmit_Status) || mContext->getStatus().isMessageMasked())
		{
			saveLogButton = mMessageBox->addButton(tr("Save Log"), QMessageBox::HelpRole);
		}
		mMessageBox->setWindowTitle(QApplication::applicationName() + QStringLiteral(" - ") + tr("Error"));
		mMessageBox->setWindowModality(Qt::ApplicationModal);
		mMessageBox->setIcon(QMessageBox::Warning);
		mMessageBox->setWindowFlags(mMessageBox->windowFlags() & ~Qt::WindowContextHelpButtonHint);
		mMessageBox->setText(message);
		mMessageBox->setStandardButtons(QMessageBox::Ok);
		mMessageBox->button(QMessageBox::Ok)->setFocus();
		do
		{
			mMessageBox->exec();
			if (mMessageBox->clickedButton() == saveLogButton)
			{
				LogFileSaveDialog().saveLogFile(mMessageBox);
			}
		}
		while (mMessageBox->clickedButton() == saveLogButton);
	}

	Q_EMIT fireUiFinished();
}


void StepErrorGui::forwardStep()
{
	Q_EMIT fireUiFinished();
}


void StepErrorGui::closeActiveDialogs()
{
	if (mMessageBox)
	{
		mMessageBox->reject();
	}
}
