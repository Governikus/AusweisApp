/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "StepAdviseUserToRemoveCardGui.h"

#include "ReaderManager.h"

#include <QAbstractButton>
#include <QCoreApplication>
#include <QIcon>

using namespace governikus;


void StepAdviseUserToRemoveCardGui::onReaderManagerSignal()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	if (!readerManager->getReaderInfo(mContext->getReaderName()).hasCard())
	{
		qDebug() << "No more ID cards found -> auto closing card reminder dialog";

		disconnect(readerManager, &ReaderManager::fireReaderAdded, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(readerManager, &ReaderManager::fireReaderRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(readerManager, &ReaderManager::fireCardInserted, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(readerManager, &ReaderManager::fireCardRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(&mMessageTimeoutTimer, &QTimer::timeout, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);

		mMessageBox->reject();
	}
}


StepAdviseUserToRemoveCardGui::StepAdviseUserToRemoveCardGui(QSharedPointer<WorkflowContext> pContext, QWidget* pMainWidget)
	: StepGui(pContext)
	, mContext(pContext)
	, mMainWidget(pMainWidget)
	, mMessageBox(nullptr)
	, mMessageTimeoutTimer()
{
}


StepAdviseUserToRemoveCardGui::~StepAdviseUserToRemoveCardGui()
{
	if (mMessageBox && mMessageBox->isActiveWindow())
	{
		mMessageBox->reject();
	}
}


void StepAdviseUserToRemoveCardGui::activate()
{
	setCancelButtonState(ButtonState::HIDDEN);

	const QString selectedReaderName = mContext->getReaderName();
	if (selectedReaderName.isEmpty())
	{
		return;
	}

	const auto readerManager = Env::getSingleton<ReaderManager>();
	ReaderInfo selectedReader = readerManager->getReaderInfo(mContext->getReaderName());
	if (selectedReader.isConnected())
	{
		if (!selectedReader.hasCard())
		{
			return;
		}

		connect(readerManager, &ReaderManager::fireReaderAdded, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		connect(readerManager, &ReaderManager::fireReaderRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		connect(readerManager, &ReaderManager::fireCardInserted, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		connect(readerManager, &ReaderManager::fireCardRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
	}
	else
	{
		mMessageTimeoutTimer.setInterval(3000);
		mMessageTimeoutTimer.setSingleShot(true);
		connect(&mMessageTimeoutTimer, &QTimer::timeout, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		mMessageTimeoutTimer.start();
	}

	// If workflow has been killed, we do not open the dialog.
	if (mContext->isWorkflowKilled())
	{
		return;
	}

	if (mMessageBox.isNull())
	{
		mMessageBox = QPointer<QMessageBox>(new QMessageBox(mMainWidget));
		mMessageBox->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
		mMessageBox->setWindowModality(Qt::ApplicationModal);
		mMessageBox->setWindowFlags(mMessageBox->windowFlags() & ~Qt::WindowContextHelpButtonHint);
		mMessageBox->setText(tr("You may now remove your ID card from the card reader."));
		mMessageBox->setIconPixmap(QIcon(QStringLiteral(":/images/npa.svg")).pixmap(32, 32));
		mMessageBox->setStandardButtons(QMessageBox::Ok);
		mMessageBox->button(QMessageBox::Ok)->setFocus();
	}

	mMessageBox->exec();
}


void StepAdviseUserToRemoveCardGui::closeActiveDialogs()
{
	if (mMessageBox)
	{
		mMessageBox->reject();
	}
}
