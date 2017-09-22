/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppQtMainWidget.h"
#include "generic/GuiUtils.h"
#include "StepErrorGui.h"

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
	// On error don't automatically close the window after the workflow is done.
	mMainWidget->setHideWindowAfterWorkflow(false);


	auto result = mContext->getResult();
	if (result.getMinor() == Result::Minor::SAL_Invalid_Key && !mContext->getCardConnection()->getReaderInfo().getCardInfo().isPinDeactivated())
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


	QString message = result.getMessage();
	if (message.isEmpty() || result.isOriginServer())
	{
		// if the message is not set, we clearly use the result minor
		// if the server sent the message, it can be in any language, so we take the result minor
		message = result.getMinorDescription();
	}
	if (message.isEmpty())
	{
		qCCritical(gui) << "No error message determined:" << result.getMajorString() << result.getMinorString() << result.getMessage();
		message = tr("Sorry, that should not have happened! Please contact the support team.");
		Q_ASSERT(!message.isEmpty());
	}


	QMessageBox::warning(mMainWidget, QCoreApplication::applicationName() + " - " + tr("Error"), message);
	Q_EMIT fireUiFinished();
}


void StepErrorGui::forwardStep()
{
	Q_EMIT fireUiFinished();
}
