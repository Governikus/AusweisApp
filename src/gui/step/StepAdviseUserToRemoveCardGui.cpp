/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StepAdviseUserToRemoveCardGui.h"

#include "PinSettingsWidget.h"
#include "ReaderManager.h"

#include <QCoreApplication>


using namespace governikus;


void StepAdviseUserToRemoveCardGui::onReaderManagerSignal()
{
	if (ReaderManager::getInstance().getReaderInfo(mContext->getReaderName()).getCardType() == CardType::NONE)
	{
		qDebug() << "No more ID cards found -> auto closing card reminder dialog";

		disconnect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
		disconnect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);

		mMessageBox->reject();
	}
}


StepAdviseUserToRemoveCardGui::StepAdviseUserToRemoveCardGui(QSharedPointer<WorkflowContext> pContext, QWidget* pMainWidget)
	: StepGui(pContext)
	, mContext(pContext)
	, mMainWidget(pMainWidget)
	, mMessageBox(nullptr)
{
}


StepAdviseUserToRemoveCardGui::~StepAdviseUserToRemoveCardGui()
{
}


void StepAdviseUserToRemoveCardGui::activate()
{
	Q_EMIT setCancelButtonState(ButtonState::HIDDEN);

	if (ReaderManager::getInstance().getReaderInfo(mContext->getReaderName()).getCardType() == CardType::NONE)
	{
		return;
	}

	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &StepAdviseUserToRemoveCardGui::onReaderManagerSignal);

	if (mMessageBox == nullptr)
	{
		mMessageBox = new QMessageBox(mMainWidget);
		mMessageBox->setWindowTitle(QCoreApplication::applicationName() + " - Information");
		mMessageBox->setWindowModality(Qt::WindowModal);
		mMessageBox->setText(tr("You may now remove your ID card from the card reader."));
		mMessageBox->setIconPixmap(QPixmap(QStringLiteral(":images/autentapp2.iconset/icon_32x32.png")));
		mMessageBox->setWindowFlags(mMessageBox->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	}

	mMessageBox->exec();
}
