/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "StepErrorGui.h"

#include "AppQtMainWidget.h"
#include "generic/GuiUtils.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QMessageBox>
#include <QTimer>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


StepErrorGui::StepErrorGui(QSharedPointer<WorkflowContext> pContext, AppQtMainWidget* const pMainWidget)
	: StepGui(pContext)
	, mContext(pContext)
	, mMainWidget(pMainWidget)
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

	QMessageBox::warning(mMainWidget, QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Error"), message);
	Q_EMIT fireUiFinished();
}


void StepErrorGui::forwardStep()
{
	Q_EMIT fireUiFinished();
}
