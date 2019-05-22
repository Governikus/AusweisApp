/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StepErrorGui.h"

#include "AppQtMainWidget.h"
#include "BuildHelper.h"
#include "EnumHelper.h"
#include "Env.h"
#include "generic/GuiUtils.h"
#include "LogFileSaveDialog.h"
#include "LogHandler.h"

#include <QApplication>
#include <QDesktopServices>
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
		QPushButton* sendEmailButton = nullptr;

		if (mContext->getStatus().is(GlobalStatus::Code::Card_Unexpected_Transmit_Status) || mContext->getStatus().isMessageMasked())
		{
			if (Env::getSingleton<LogHandler>()->useLogfile())
			{
				saveLogButton = mMessageBox->addButton(tr("Save Log"), QMessageBox::HelpRole);
			}
			sendEmailButton = mMessageBox->addButton(tr("Send email"), QMessageBox::HelpRole);
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
			else if (mMessageBox->clickedButton() == sendEmailButton)
			{
				const GlobalStatus status = mContext->getStatus();
				//: Subject from error report mail
				QString mailSubject = tr("AusweisApp2 error report - %1").arg(status.toErrorDescription());
				QString mailBody = generateMailBody(status);
				QString url = QStringLiteral("mailto:support@ausweisapp.de?subject=%1&body=%2").arg(mailSubject, mailBody);

				QDesktopServices::openUrl(url);
			}
		}
		while (mMessageBox->clickedButton() == saveLogButton || mMessageBox->clickedButton() == sendEmailButton);
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


QString StepErrorGui::generateMailBody(const GlobalStatus& pStatus) const
{
	const auto& logHandler = Env::getSingleton<LogHandler>();
	QStringList mailBody(tr("Please describe the error that occurred."));

	if (logHandler->useLogfile())
	{
		mailBody << tr("You may want to attach the logfile which can be saved from the error dialog.");
	}

	const QString newLine = QStringLiteral("\n");
	mailBody << newLine;

	const auto& systemInfo = BuildHelper::getInformationHeader();
	for (const auto& info : systemInfo)
	{
		QString first = info.first;
		QString second = info.second;

		first.replace(QStringLiteral("&"), QStringLiteral("%26"));
		second.replace(QStringLiteral("&"), QStringLiteral("%26"));

		mailBody << first + QStringLiteral(": ") + second;
	}

	mailBody << newLine + tr("Error code") + QLatin1Char(':');
	mailBody << getEnumName(pStatus.getStatusCode());

	if (logHandler->hasCriticalLog())
	{
		const QString criticalMessages = QString::fromUtf8(logHandler->getCriticalLogWindow());
		mailBody << newLine + tr("Critical errors:") + newLine + criticalMessages;
	}

	return mailBody.join(newLine);
}
