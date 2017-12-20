/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "StepAdviseUserToRemoveCardGui.h"

#include "PinSettingsWidget.h"
#include "ReaderManager.h"

#include <QCoreApplication>
#include <QIcon>


using namespace governikus;


void StepAdviseUserToRemoveCardGui::onReaderManagerSignal()
{
	if (!ReaderManager::getInstance().getReaderInfo(mContext->getReaderName()).hasCard())
	{
		qDebug() << "No more ID cards found -> auto closing card reminder dialog";

		disconnect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
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
}


void StepAdviseUserToRemoveCardGui::activate()
{
	setCancelButtonState(ButtonState::HIDDEN);

	const QString selectedReaderName = mContext->getReaderName();
	if (selectedReaderName.isEmpty())
	{
		return;
	}

	ReaderInfo selectedReader = ReaderManager::getInstance().getReaderInfo(mContext->getReaderName());
	if (selectedReader.isConnected())
	{
		if (!selectedReader.hasCard())
		{
			return;
		}

		connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		connect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		connect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
	}
	else
	{
		mMessageTimeoutTimer.setInterval(3000);
		mMessageTimeoutTimer.setSingleShot(true);
		connect(&mMessageTimeoutTimer, &QTimer::timeout, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		mMessageTimeoutTimer.start();
	}

	if (mMessageBox == nullptr)
	{
		mMessageBox = new QMessageBox(mMainWidget);
		mMessageBox->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
		mMessageBox->setWindowModality(Qt::WindowModal);
		mMessageBox->setWindowFlags(mMessageBox->windowFlags() & ~Qt::WindowContextHelpButtonHint);
		mMessageBox->setText(tr("You may now remove your ID card from the card reader."));
		mMessageBox->setIconPixmap(QIcon(QStringLiteral(":/images/npa.svg")).pixmap(32, 32));
	}

	mMessageBox->exec();
}
