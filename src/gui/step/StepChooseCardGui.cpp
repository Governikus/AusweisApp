/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StepChooseCardGui.h"

#include "generic/BusyOverlayContainer.h"
#include "generic/SmartCardUtil.h"
#include "GuiProfile.h"
#include "step/AuthenticateStepsWidget.h"

#include <QLoggingCategory>
#include <QPushButton>
#include <QSharedPointer>
#include <QVector>
#include <QWidget>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


StepChooseCardGui::StepChooseCardGui(const QSharedPointer<WorkflowContext>& pContext, AuthenticateStepsWidget* pStepsWidget)
	: StepGui(pContext)
	, mInformationMessageBox(new QMessageBox(pStepsWidget))
	, mDiagnosisGui(new DiagnosisGui(pStepsWidget))
	, mReaderDriverGui(new ReaderDriverGui(pStepsWidget))
	, mCancelButton(nullptr)
	, mReaderDriverButton(nullptr)
	, mDiagnosisButton(nullptr)
	, mStepsWidget(pStepsWidget)
	, mContext(pContext)
{
	mInformationMessageBox->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - Information"));
	mInformationMessageBox->setWindowModality(Qt::WindowModal);
	mInformationMessageBox->setWindowFlags(mInformationMessageBox->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	mCancelButton = mInformationMessageBox->addButton(tr("Cancel"), QMessageBox::NoRole);
	mDiagnosisButton = mInformationMessageBox->addButton(tr("Diagnosis"), QMessageBox::YesRole);
	//mReaderDriverButton = mInformationMessageBox->addButton(tr("Drivers"), QMessageBox::YesRole);

	connect(mDiagnosisGui, &DiagnosisGui::fireFinished, this, &StepChooseCardGui::onReaderManagerSignal);
	connect(mReaderDriverGui, &ReaderDriverGui::fireFinished, this, &StepChooseCardGui::onReaderManagerSignal);
}


StepChooseCardGui::~StepChooseCardGui()
{
}


void StepChooseCardGui::activate()
{
	Q_EMIT setCancelButtonState(ButtonState::ENABLED);

	mStepsWidget->setCurrentWidget(mStepsWidget->getProcessingPage());
	mStepsWidget->getProcessingPage()->startAnimation();

	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderEvent, this, &StepChooseCardGui::onReaderManagerSignal);
	onReaderManagerSignal();
}


void StepChooseCardGui::deactivate()
{
	disconnect(&ReaderManager::getInstance(), &ReaderManager::fireReaderEvent, this, &StepChooseCardGui::onReaderManagerSignal);

	mStepsWidget->getProcessingPage()->stopAnimation();
}


QString StepChooseCardGui::getCurrentReaderImage(const QVector<ReaderInfo>& pReaderInfos)
{
	QVector<ReaderType> knownReaderTypes;
	for (const ReaderInfo& readerInfo : pReaderInfos)
	{
		if (!knownReaderTypes.contains(readerInfo.getReaderType()))
		{
			knownReaderTypes.append(readerInfo.getReaderType());
		}
	}

	if (knownReaderTypes.size() == 1)
	{
		return SmartCardUtil::getReaderEmptyIconPath(knownReaderTypes.at(0));
	}
	else if (knownReaderTypes.size() > 1)
	{
		return SmartCardUtil::getMultipleReaderIconPath();
	}

	return SmartCardUtil::getReaderEmptyIconPath(ReaderType::UNKNOWN);
}


void StepChooseCardGui::updateErrorMessage(const QString& pTitle, const QString& pMessage, bool closeErrorMessage)
{
	if (closeErrorMessage)
	{
		mDiagnosisGui->deactivate();
		mReaderDriverGui->deactivate();
		mInformationMessageBox->done(QMessageBox::InvalidRole);
		return;
	}

	QString iconPath = getCurrentReaderImage(ReaderManager::getInstance().getReaderInfos(ReaderManagerPlugInType::PCSC));
	if (iconPath.isEmpty() || GuiProfile::getProfile().isSmallScreen())
	{
		mInformationMessageBox->setIcon(QMessageBox::Information);
	}
	else
	{
		mInformationMessageBox->setIconPixmap(QPixmap(iconPath).scaledToWidth(200, Qt::SmoothTransformation));
	}
	mInformationMessageBox->setText(QStringLiteral("<b>%1</b>").arg(pTitle));
	mInformationMessageBox->setInformativeText(pMessage);

	if (mInformationMessageBox->isVisible())
	{
		// we just updated the values, no need to call exec.
		return;
	}

	if (mInformationMessageBox->exec() != QMessageBox::InvalidRole)
	{
		if (mInformationMessageBox->clickedButton() == mCancelButton)
		{
			fireCancelled();
		}
		else if (mInformationMessageBox->clickedButton() == mDiagnosisButton)
		{
			mDiagnosisGui->activate();
		}
		// else if (mInformationMessageBox->clickedButton() == mReaderDriverButton)
		// {
		//  mReaderDriverGui->activate();
		// }
	}
	// else: dialog was closed by an onErrorMessage(..., true) call (i.e. card found)
}


void StepChooseCardGui::onReaderManagerSignal()
{
	const auto readers = ReaderManager::getInstance().getReaderInfos(ReaderManagerPlugInType::PCSC);
	QVector<ReaderInfo> readersWithNpa;
	for (const auto& readerInfo : readers)
	{
		if (readerInfo.getCardType() == CardType::EID_CARD)
		{
			readersWithNpa << readerInfo;
		}
	}

	if (readers.size() == 0)
	{
		updateErrorMessage(tr("No card reader detected. Please make sure that a card reader is connected."),
				tr("If you need help or have problems with your card reader click on the"
				   " \"Diagnosis\" button for further information."),
				false);
		return;
	}

	if (readersWithNpa.size() == 0)
	{
		updateErrorMessage(tr("Please place an ID card on the card reader."),
				tr("If you have already placed an ID card on your card reader, click on \"Diagnosis\""
				   " for further information."),
				false);
	}
	else if (readersWithNpa.size() > 1)
	{
		updateErrorMessage(tr("Please place only one ID card on the card reader."),
				tr("Please make sure that only one card reader with an ID card on it is connected to"
				   " your computer. If you have already placed an ID card on your card reader, click"
				   " on \"Diagnosis\" for further information."),
				false);
	}
	else
	{
		updateErrorMessage(QString(), QString(), true);
	}
}
