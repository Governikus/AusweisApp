/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "SetupAssistantWizard.h"

#include "AppSettings.h"
#include "Env.h"
#include "generic/HelpAction.h"
#include "ReaderDeviceWidget.h"
#include "ReaderInfo.h"
#include "ReaderManager.h"

#include <QCoreApplication>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMovie>
#include <QPixmap>
#include <QTableWidget>
#include <QVariant>
#include <QWizardPage>

using namespace governikus;

CardReaderPage::CardReaderPage(const QString& pTitle, const QString& pAccessibleName)
	: mWidget(new ReaderDeviceWidget(this))
{
	setTitle(pTitle);
	mWidget->prependAccessibleName(pAccessibleName);

	QVBoxLayout* cardReaderPageVLayout = new QVBoxLayout(this);
	cardReaderPageVLayout->addWidget(mWidget);
}


SetupAssistantWizard::SetupAssistantWizard(QWidget* pParent)
	: QWizard(pParent)
	, mPageCount(3)
	, mSaveHistoryCheckBox(new QCheckBox(this))
	, mChangeTransportPinButton()
{
#ifdef Q_OS_MACOS
	static const int MIN_HEIGHT = 700;
#else
	static const int MIN_HEIGHT = 500;
#endif

	setObjectName(QStringLiteral("setupAssistant"));
	installEventFilter(this);
	setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("setup assistant"));
	setMinimumSize(700, MIN_HEIGHT);
	setWizardStyle(QWizard::ClassicStyle);
	setWindowModality(Qt::WindowModal);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setOption(QWizard::NoCancelButton, false);
	setAttribute(Qt::WA_DeleteOnClose);

	addPage(createWizardInitialPinPage());
	addPage(createWizardCardReaderPage());
	addPage(createConclusionPage());
}


SetupAssistantWizard::~SetupAssistantWizard()
{
}


QString SetupAssistantWizard::createAccessibleName(const QString& pName, const QString& pText) const
{
	const QString& stepIndex = QString::number(pageIds().size() + 1);
	const QString& pageCount = QString::number(mPageCount);

	auto result = pName + QStringLiteral(". ") + tr("Step %1 of %2").arg(stepIndex, pageCount) + QStringLiteral(". ") + pText;
	return result.remove(QLatin1Char('"')).trimmed();
}


QString SetupAssistantWizard::createTitle(const QString& pName) const
{
	const QString& stepIndex = QString::number(pageIds().size() + 1);
	const QString& pageCount = QString::number(mPageCount);

	return QStringLiteral("<div align=\"right\">")
		   + tr("Step %1 of %2").arg(stepIndex, pageCount)
		   + QStringLiteral("</div><div align=\"left\">%1</div>").arg(pName);
}


QString SetupAssistantWizard::createDescription(const QString& pTitle, const QString& pSummary) const
{
	return QStringLiteral("<br><b>%1</b><br><br>%2").arg(pTitle, pSummary);
}


QWizardPage* SetupAssistantWizard::createWizardInitialPinPage()
{
	QWizardPage* initialPinPage = new QWizardPage;
	const auto& introduction = tr("Introduction");
	initialPinPage->setTitle(createTitle(introduction));

	const auto& welcome = tr("Welcome to the AusweisApp2 setup assistant."
							 " This assistant will guide you through the setup process in %1 steps."
							 " You can cancel the setup assistant at any time. To restart it, go to the tab \"Help\" and select \"Setup assistant\".").arg(mPageCount);
	QLabel* label = new QLabel(welcome);
	label->setWordWrap(true);
	label->setFocusPolicy(Qt::TabFocus);
	label->setAccessibleName(createAccessibleName(introduction, welcome));

	QVBoxLayout* initialPinPageLayout = new QVBoxLayout;
	initialPinPageLayout->addWidget(label);

	const auto& historyTitle = tr("History");
	const auto& historySummary = tr("Do you want to save the history of your authentications?");
	QLabel* historyDescLabel = new QLabel(createDescription(historyTitle, historySummary));
	historyDescLabel->setWordWrap(true);
	historyDescLabel->setFocusPolicy(Qt::TabFocus);
	historyDescLabel->setAccessibleName(createAccessibleName(historyTitle, historySummary));

	initialPinPageLayout->addWidget(historyDescLabel);

	QWidget* saveHistoryWidget = new QWidget(this);

	QSizePolicy saveHistorySizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	saveHistorySizePolicy.setHorizontalStretch(0);
	saveHistorySizePolicy.setVerticalStretch(0);
	saveHistorySizePolicy.setHeightForWidth(saveHistoryWidget->sizePolicy().hasHeightForWidth());
	saveHistoryWidget->setSizePolicy(saveHistorySizePolicy);

	QFormLayout* saveHistoryFormLayout = new QFormLayout(saveHistoryWidget);
	saveHistoryFormLayout->setHorizontalSpacing(30);
	saveHistoryFormLayout->setContentsMargins(11, 11, 11, 11);
	saveHistoryFormLayout->setContentsMargins(0, 5, 0, 20);

	QLabel* saveHistoryLabel = new QLabel();
	saveHistoryLabel->setFocusPolicy(Qt::TabFocus);
	saveHistoryLabel->setText(historyTitle + QLatin1Char(':'));

	mSaveHistoryCheckBox->setText(tr("save"));
	mSaveHistoryCheckBox->setAccessibleName(tr("save history"));
	mSaveHistoryCheckBox->setChecked(AppSettings::getInstance().getHistorySettings().isEnabled());

	saveHistoryFormLayout->setWidget(0, QFormLayout::LabelRole, saveHistoryLabel);
	saveHistoryFormLayout->setWidget(0, QFormLayout::FieldRole, mSaveHistoryCheckBox);

	initialPinPageLayout->addWidget(saveHistoryWidget);

	initialPinPage->setLayout(initialPinPageLayout);

	return initialPinPage;
}


QWizardPage* SetupAssistantWizard::createWizardCardReaderPage()
{
	const auto& title = tr("Card Readers");
	const auto& titleField = createTitle(title);
	const auto& accessibleField = createAccessibleName(title);
	return new CardReaderPage(titleField, accessibleField);
}


QWizardPage* SetupAssistantWizard::createConclusionPage()
{
	QWizardPage* conclusionPage = new QWizardPage;
	const auto& almostDone = tr("Almost done!");
	conclusionPage->setTitle(createTitle(almostDone));

	QVBoxLayout* conclusionPageVLayout = new QVBoxLayout(conclusionPage);


	const auto& title = tr("Personal 6 - digit PIN");
	const auto& desc = tr("Prior to the first use of the online identification function, you have to replace the transport PIN by an individual 6-digit PIN. "
						  "The transport PIN was sent to you by postal mail.");
	QLabel* transportPinLabel = new QLabel(createDescription(title, desc));
	transportPinLabel->setWordWrap(true);
	transportPinLabel->setFocusPolicy(Qt::TabFocus);
	transportPinLabel->setAccessibleName(createAccessibleName(title, desc));

	conclusionPageVLayout->addWidget(transportPinLabel);

	mChangeTransportPinButton = new QPushButton(conclusionPage);
	mChangeTransportPinButton->setText(tr("Set individual PIN"));
	connect(mChangeTransportPinButton.data(), &QAbstractButton::clicked, this, &SetupAssistantWizard::onChangeTransportPinButtonPressed);

	QSizePolicy transportPinSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	transportPinSizePolicy.setHorizontalStretch(0);
	transportPinSizePolicy.setVerticalStretch(0);
	transportPinSizePolicy.setHeightForWidth(mChangeTransportPinButton->sizePolicy().hasHeightForWidth());
	mChangeTransportPinButton->setSizePolicy(transportPinSizePolicy);

	conclusionPageVLayout->addWidget(mChangeTransportPinButton);

	const QString onlineHelpUrl = HelpAction::getOnlineUrl(QStringLiteral("setupAssistantSetupCompleted"));
	const auto conclusionDesc =
			tr("AusweisApp2 is now ready for use."
			   " For more information on the software or the online identification function, visit the %1online help%2.")
			.arg(QStringLiteral("<a href=\"%1\">").arg(onlineHelpUrl), QStringLiteral("</a>"));
	QLabel* conclusionDescLabel = new QLabel(QStringLiteral("<br>") + conclusionDesc);
	conclusionDescLabel->setWordWrap(true);
	conclusionDescLabel->setFocusPolicy(Qt::TabFocus);
	conclusionDescLabel->setAccessibleName(createAccessibleName(almostDone, conclusionDesc));
	conclusionDescLabel->setOpenExternalLinks(true);

	conclusionPageVLayout->addWidget(conclusionDescLabel);

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	conclusionPageVLayout->addItem(verticalSpacer);

	return conclusionPage;
}


void SetupAssistantWizard::onChangeTransportPinButtonPressed()
{
	auto& historySettings = AppSettings::getInstance().getHistorySettings();
	historySettings.setEnabled(mSaveHistoryCheckBox->isChecked());
	historySettings.save();
	Q_EMIT fireChangePinButtonClicked();
	close();
}


void SetupAssistantWizard::accept()
{
	auto& historySettings = AppSettings::getInstance().getHistorySettings();
	historySettings.setEnabled(mSaveHistoryCheckBox->isChecked());
	historySettings.save();
	hide();
}


bool SetupAssistantWizard::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp(objectName());
			return true;
		}
	}
	return QWizard::eventFilter(pObject, pEvent);
}
