/*!
 * SetupAssistantWizard.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "generic/GuiUtils.h"
#include "generic/HelpAction.h"
#include "generic/SmartCardUtil.h"
#include "ReaderInfo.h"
#include "ReaderManager.h"
#include "SetupAssistantWizard.h"

#include "ReaderDriverWidget.h"

#include <QCoreApplication>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMovie>
#include <QPixmap>
#include <QTableWidget>
#include <QVariant>
#include <QWizardPage>

using namespace governikus;


CardReaderPage::CardReaderPage(int pStepIndex, int pPageCount, SetupAssistantWizard* pWizard)
	: mWidget(new ReaderDriverWidget(this))
{
	//  QWizardPage* cardReaderPage = new QWizardPage;
	this->setTitle(tr("<div align=\"right\">Step %1 of %2</div><div align=\"left\">Card Reader</div>").arg(pStepIndex).arg(pPageCount));

	QVBoxLayout* cardReaderPageVLayout = new QVBoxLayout(this);
	QLabel* cardReaderlDescLabel = new QLabel(tr("In order to be able to use the online identification function with AusweisApp2, you need a card reader. Please make sure that you have connected a suitable card reader and that the drivers required for your card reader are installed on your system. The following list shows all detected card readers and the status of the corresponding driver."));
	cardReaderlDescLabel->setWordWrap(true);
	cardReaderlDescLabel->setFocusPolicy(Qt::TabFocus);
	cardReaderlDescLabel->setAccessibleName(tr("Card Reader Step %1 of %2. In order to be able to use the online identification function with AusweisApp2, you need a card reader. Please make sure that you have connected a suitable card reader and that the drivers required for your card reader are installed on your system. The following list shows all detected card readers and the status of the corresponding driver.").arg(pStepIndex).arg(pPageCount));

	GuiUtils::updateFontSize(cardReaderlDescLabel);

	cardReaderPageVLayout->addWidget(cardReaderlDescLabel);
	cardReaderPageVLayout->addWidget(mWidget);

	connect(pWizard, &SetupAssistantWizard::currentIdChanged, mWidget, &ReaderDriverWidget::onUpdateContent);
}


CardReaderPage::~CardReaderPage()
{
	// No need to delete mWidget because it is handled by Qt
}


void CardReaderPage::initializePage()
{
	// Update widget after it has become visible
	// Calling onUpdateButtonClicked directly won't work: the reader icon will not be shown
	QMetaObject::invokeMethod(mWidget, "updateContent", Qt::QueuedConnection);
}


SetupAssistantWizard::SetupAssistantWizard(QWidget* pParent)
	: QWizard(pParent)
	, mPageCount(0)
	, mNoScriptFinder()
	, mSaveHistoryCheckBox(new QCheckBox(this))
	, mChangeTransportPinButton()
{
	setObjectName(QStringLiteral("setupAssistant"));
	installEventFilter(this);
	setWindowTitle(QCoreApplication::applicationName() + tr(" - setup assistant"));
	setMinimumSize(700, 450);
	setWizardStyle(QWizard::ClassicStyle);
	setWindowModality(Qt::WindowModal);
	setOption(QWizard::NoCancelButton, false);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	mPageCount = 3;
	if (mNoScriptFinder.isExtensionFound())
	{
		++mPageCount;
	}

	addPage(createWizardInitialPinPage());
	if (mNoScriptFinder.isExtensionFound())
	{
		addPage(createWizardNoScriptExtensionPage());
	}
	addPage(createWizardCardReaderPage());
	addPage(createConclusionPage());
}


SetupAssistantWizard::~SetupAssistantWizard()
{
}


QWizardPage* SetupAssistantWizard::createWizardInitialPinPage()
{
	QWizardPage* initialPinPage = new QWizardPage;
	initialPinPage->setTitle(tr("<div align=\"right\">Step %1 of %2</div><div align=\"left\">Introduction</div>").arg(this->pageIds().size() + 1).arg(mPageCount));

	QLabel* label = new QLabel(tr("Welcome to the AusweisApp2 setup assistant. This assistant will guide you through the setup process in %1 steps. The setup assistant can be cancelled at any time and can be started again later from the Help menu.").arg(mPageCount));
	label->setWordWrap(true);
	label->setFocusPolicy(Qt::TabFocus);
	label->setAccessibleName(tr("Introduction Step %1 of %2. Welcome to the AusweisApp2 setup assistant. This assistant will guide you through the setup process in %2 steps. The setup assistant can be cancelled at any time and can be started again later from the Help menu.").arg(this->pageIds().size() + 1).arg(mPageCount));

	GuiUtils::updateFontSize(label);

	QVBoxLayout* initialPinPageLayout = new QVBoxLayout;
	initialPinPageLayout->addWidget(label);


	QLabel* historyDescLabel = new QLabel(tr("<br><b>History</b><br><br>AusweisApp2 offers saving the course of your authentications in a history. Subsequently you can activate this option."));
	historyDescLabel->setWordWrap(true);
	historyDescLabel->setFocusPolicy(Qt::TabFocus);
	historyDescLabel->setAccessibleName(tr("History. AusweisApp2 offers saving the course of your authentications in a history. Subsequently you can activate this option."));

	GuiUtils::updateFontSize(historyDescLabel);

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
	saveHistoryLabel->setText(tr("History:"));

	GuiUtils::updateFontSize(saveHistoryLabel);

	mSaveHistoryCheckBox->setText(tr("save"));
	mSaveHistoryCheckBox->setAccessibleName(tr("save history"));
	mSaveHistoryCheckBox->setChecked(AppSettings::getInstance().getHistorySettings().isEnabled());

	GuiUtils::updateFontSize(mSaveHistoryCheckBox);

	saveHistoryFormLayout->setWidget(0, QFormLayout::LabelRole, saveHistoryLabel);
	saveHistoryFormLayout->setWidget(0, QFormLayout::FieldRole, mSaveHistoryCheckBox);

	initialPinPageLayout->addWidget(saveHistoryWidget);

	initialPinPage->setLayout(initialPinPageLayout);

	return initialPinPage;
}


QWizardPage* SetupAssistantWizard::createWizardCardReaderPage()
{
	//	return new CardReaderPage(pageIds().size() + 1, mPageCount, this);

	QWizardPage* cardReaderPage = new QWizardPage;
	cardReaderPage->setTitle(tr("<div align=\"right\">Step %1 of %2</div><div align=\"left\">Card Reader</div>").arg(this->pageIds().size() + 1).arg(mPageCount));

	QVBoxLayout* cardReaderPageVLayout = new QVBoxLayout(cardReaderPage);
	QLabel* cardReaderlDescLabel = new QLabel(tr("In order to be able to use the online identification function with AusweisApp2, you need a card reader. Please make sure that you have connected a suitable card reader and that the drivers required for your card reader are installed on your system. The following list shows all suitable card readers. For each card reader you will find a link to the manufacturer's website, where you can download the most recent drivers."));
	cardReaderlDescLabel->setWordWrap(true);
	cardReaderlDescLabel->setFocusPolicy(Qt::TabFocus);
	cardReaderlDescLabel->setAccessibleName(tr("Card Reader Step %1 of %2. In order to be able to use the online identification function with AusweisApp2, you need a card reader. Please make sure that you have connected a suitable card reader and that the drivers required for your card reader are installed on your system. The following list shows all suitable card readers. For each card reader you will find a link to the manufacturer's website, where you can download the most recent drivers.").arg(this->pageIds().size() + 1).arg(mPageCount));

	GuiUtils::updateFontSize(cardReaderlDescLabel);

	cardReaderPageVLayout->addWidget(cardReaderlDescLabel);

	QGridLayout* readerGridLayout = new QGridLayout();
	readerGridLayout->setSpacing(6);
	readerGridLayout->setContentsMargins(0, 0, 0, 0);

	//table
	QTableWidget* cardReaderTable = new QTableWidget();
	cardReaderTable->setFrameShadow(QFrame::Plain);
	cardReaderTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	cardReaderTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	cardReaderTable->setAutoScroll(true);
	cardReaderTable->setLineWidth(1);
	cardReaderTable->setSelectionMode(QAbstractItemView::SingleSelection);
	cardReaderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	cardReaderTable->setShowGrid(false);
	cardReaderTable->setTabKeyNavigation(false);

	cardReaderTable->horizontalHeader()->setVisible(false);
	cardReaderTable->verticalHeader()->setVisible(false);

	//columns
	cardReaderTable->setColumnCount(3);
	QTableWidgetItem* readerHeaderItem = new QTableWidgetItem();
	readerHeaderItem->setText(tr("Reader"));
	cardReaderTable->setHorizontalHeaderItem(0, readerHeaderItem);

	QTableWidgetItem* manufacturerHeaderItem = new QTableWidgetItem();
	manufacturerHeaderItem->setText(tr("Name / Manufacturer"));
	cardReaderTable->setHorizontalHeaderItem(1, manufacturerHeaderItem);

	QTableWidgetItem* webPageHeaderItem = new QTableWidgetItem();
	webPageHeaderItem->setText(tr("Web page"));
	cardReaderTable->setHorizontalHeaderItem(2, webPageHeaderItem);

	//rows
	cardReaderTable->setRowCount(13);
	addReader(cardReaderTable, 0, ReaderType::ACS_ACR1281_PICC_Reader);
	addReader(cardReaderTable, 1, ReaderType::FEIG_OBID_myAXXESS_basic);
	addReader(cardReaderTable, 2, ReaderType::Gemalto_Prox_DU);
	addReader(cardReaderTable, 3, ReaderType::Gemalto_Prox_SU);
	addReader(cardReaderTable, 4, ReaderType::OMNIKEY_4121_CL);
	addReader(cardReaderTable, 5, ReaderType::OMNIKEY_CardMan_5x21_CL);
	addReader(cardReaderTable, 6, ReaderType::KOBIL_IDToken);
	addReader(cardReaderTable, 7, ReaderType::REINER_cyberJack_RFID_basis);
	addReader(cardReaderTable, 8, ReaderType::REINER_cyberJack_RFID_standard);
	addReader(cardReaderTable, 9, ReaderType::REINER_cyberJack_RFID_komfort);
	addReader(cardReaderTable, 10, ReaderType::REINER_cyberJack_wave);
	addReader(cardReaderTable, 11, ReaderType::SCM_SCL011_Contactless_Reader);
	addReader(cardReaderTable, 12, ReaderType::SCM_SDI011);

	cardReaderTable->resizeRowsToContents();
	cardReaderTable->horizontalHeader()->setStretchLastSection(true);

	readerGridLayout->addWidget(cardReaderTable, 0, 0, 1, 1);

	cardReaderPageVLayout->addLayout(readerGridLayout);

	return cardReaderPage;
}


void SetupAssistantWizard::addReader(QTableWidget* pTableWidget, int pRow, ReaderType pReaderType)
{
	QLabel* manufacturerLabel = new QLabel();
	manufacturerLabel->setWordWrap(true);
	manufacturerLabel->setMargin(3);
	manufacturerLabel->setFocusPolicy(Qt::TabFocus);

	QString manufacturerWebPage;

	switch (pReaderType)
	{
		case ReaderType::REINER_cyberJack_RFID_komfort:
			manufacturerLabel->setText(QStringLiteral("Reiner SCT<br>cyberJack RFID komfort"));
			manufacturerWebPage = QStringLiteral("https://www.reiner-sct.com/support/support-anfrage/?os=Windows&productGroup=77304735&product=77304822&q=driver#choice5");
			break;

		case ReaderType::REINER_cyberJack_RFID_standard:
			manufacturerLabel->setText(QStringLiteral("Reiner SCT<br>cyberJack RFID standard"));
			manufacturerWebPage = QStringLiteral("https://www.reiner-sct.com/support/support-anfrage/?os=Windows&productGroup=77304735&product=77304820&q=driver#choice5");
			break;

		case ReaderType::REINER_cyberJack_RFID_basis:
			manufacturerLabel->setText(QStringLiteral("Reiner SCT<br>cyberJack RFID basis"));
			manufacturerWebPage = QStringLiteral("https://www.reiner-sct.com/support/support-anfrage/?os=Windows&productGroup=77304735&product=77304856&q=driver#choice5");
			break;

		case ReaderType::REINER_cyberJack_wave:
			manufacturerLabel->setText(QStringLiteral("Reiner SCT<br>cyberJack wave"));
			manufacturerWebPage = QStringLiteral("https://www.reiner-sct.com/support/support-anfrage/?os=Windows&productGroup=77304735&product=77304828&q=driver#choice5");
			break;

		case ReaderType::SCM_SCL011_Contactless_Reader:
			manufacturerLabel->setText(QStringLiteral("Identive<br>Identive SCL011"));
			manufacturerWebPage = QStringLiteral("https://support.identiv.com/scl010-scl011/");
			break;

		case ReaderType::SCM_SDI010:
		case ReaderType::SCM_SDI011:
			manufacturerLabel->setText(QStringLiteral("Identive<br>Identive SDI011"));
			manufacturerWebPage = QStringLiteral("https://support.identiv.com/sdi010-011/");
			break;

		case ReaderType::KOBIL_IDToken:
			manufacturerLabel->setText(QStringLiteral("Kobil<br>Kobil ID Token"));
			manufacturerWebPage = QStringLiteral("https://www.kobil.com/de/support_de/#drivers_de");
			break;

		case ReaderType::ACS_ACR1281_PICC_Reader:
			manufacturerLabel->setText(QStringLiteral("ACS<br>ACR1281U"));
			manufacturerWebPage = QStringLiteral("http://www.idvation.com/support/faq/4-contactless-readers/acr128/?tx_irfaq_pi1%5Bcat%5D=10");
			break;

		case ReaderType::OMNIKEY_CardMan_5x21_CL:
			manufacturerLabel->setText(QStringLiteral("HID Global GmbH<br>OMNIKEY 5321 V2"));
			manufacturerWebPage = QStringLiteral("https://www.hidglobal.com/drivers?field_brand_tid=24&product_id=4077&os=All");

			break;

		case ReaderType::OMNIKEY_4121_CL:
			manufacturerLabel->setText(QStringLiteral("HID Global GmbH<br>OMNIKEY 4121 CL"));
			manufacturerWebPage = QStringLiteral("https://www.hidglobal.com/drivers?field_brand_tid=24&product_id=4077&os=All");

			break;

		case ReaderType::FEIG_OBID_myAXXESS_basic:
			manufacturerLabel->setText(QStringLiteral("FEIG electronic GmbH<br>myAXXESS basic"));
			manufacturerWebPage = QStringLiteral("http://www.feig.de/downloads/");
			break;

		case ReaderType::Gemalto_Prox_SU:
			manufacturerLabel->setText(QStringLiteral("Gemalto<br>Prox-SU"));
			manufacturerWebPage = QStringLiteral("http://support.gemalto.com/index.php?id=prox-du_prox-su#.WOIQijvyjyQ");
			break;

		case ReaderType::Gemalto_Prox_DU:
			manufacturerLabel->setText(QStringLiteral("Gemalto<br>Prox-DU"));
			manufacturerWebPage = QStringLiteral("http://support.gemalto.com/index.php?id=prox-du_prox-su#.WOIQijvyjyQ");
			break;

		case ReaderType::UNKNOWN:
			break;
	}

	QLabel* readerIconLabel = new QLabel();
	readerIconLabel->setAccessibleName(tr("reader icon"));
	readerIconLabel->setFocusPolicy(Qt::TabFocus);
	readerIconLabel->setAlignment(Qt::AlignCenter);
	QPixmap emptyReaderPixmap(SmartCardUtil::getReaderEmptyIconPath(pReaderType));
	readerIconLabel->setPixmap(emptyReaderPixmap.scaledToWidth(100, Qt::SmoothTransformation));

	pTableWidget->setCellWidget(pRow, 0, readerIconLabel);

	manufacturerLabel->setAccessibleName(tr("Name / Manufacturer") + manufacturerLabel->text());

	GuiUtils::updateFontSize(manufacturerLabel);

	pTableWidget->setCellWidget(pRow, 1, manufacturerLabel);

	QLabel* webPageLabel = new QLabel("<a href=\"" + manufacturerWebPage + "\">" + manufacturerWebPage + "</a>");
	webPageLabel->setWordWrap(true);
	webPageLabel->setFocusPolicy(Qt::TabFocus);
	webPageLabel->setTextFormat(Qt::RichText);
	webPageLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
	webPageLabel->setOpenExternalLinks(true);

	GuiUtils::updateFontSize(webPageLabel);

	pTableWidget->setCellWidget(pRow, 2, webPageLabel);
}


QWizardPage* SetupAssistantWizard::createWizardNoScriptExtensionPage()
{
	QWizardPage* wizardPage = new QWizardPage;
	wizardPage->setTitle(tr("<div align=\"right\">Step %1 of %2</div><div align=\"left\">Firefox extension NoScript</div>").arg(this->pageIds().size() + 1).arg(mPageCount));

	QString descriptionText = tr("The Firefox browser extension NoScript was found on your computer."
								 " The configuration of this extension may block authentication requests from being passed to %1."
								 " Refer to the online help for further details.").arg(QCoreApplication::applicationName());
	QLabel* label = new QLabel(descriptionText);
	label->setWordWrap(true);
	label->setFocusPolicy(Qt::TabFocus);
	label->setAccessibleName(tr("Firefox extension NoScript Step %1 of %2.").arg(this->pageIds().size() + 1).arg(mPageCount) + descriptionText);

	GuiUtils::updateFontSize(label);

	QVBoxLayout* pageLayout = new QVBoxLayout;
	pageLayout->addWidget(label);
	wizardPage->setLayout(pageLayout);

	return wizardPage;
}


QWizardPage* SetupAssistantWizard::createConclusionPage()
{
	QWizardPage* conclusionPage = new QWizardPage;
	conclusionPage->setTitle(tr("<div align=\"right\">Step %1 of %2</div><div align=\"left\">Almost done!</div>").arg(this->pageIds().size() + 1).arg(mPageCount));

	QVBoxLayout* conclusionPageVLayout = new QVBoxLayout(conclusionPage);

	QLabel* transportPinLabel = new QLabel(tr("<b>Personal 6-digit PIN</b>"
											  "<br><br>"
											  "Prior to the first use of the online identification function you have to replace the transport PIN by an individual 6-digit PIN. The AusweisApp's PIN management offers this function. For replacing the transport PIN you need the letter sent to you by Bundesdruckerei GmbH on behalf of your citizen centre."));
	transportPinLabel->setWordWrap(true);
	transportPinLabel->setFocusPolicy(Qt::TabFocus);
	transportPinLabel->setAccessibleName(tr("Personal 6-digit PIN. Prior to the first use of the online identification function you have to replace the transport PIN by an individual 6-digit PIN. The AusweisApp's PIN management offers this function. For replacing the transport PIN you need the letter sent to you by Bundesdruckerei GmbH on behalf of your citizen centre."));

	GuiUtils::updateFontSize(transportPinLabel);

	conclusionPageVLayout->addWidget(transportPinLabel);

	mChangeTransportPinButton = new QPushButton(conclusionPage);
	mChangeTransportPinButton->setText(tr("Change PIN"));
	connect(mChangeTransportPinButton.data(), &QAbstractButton::clicked, this, &SetupAssistantWizard::onChangeTransportPinButtonPressed);

	QSizePolicy transportPinSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	transportPinSizePolicy.setHorizontalStretch(0);
	transportPinSizePolicy.setVerticalStretch(0);
	transportPinSizePolicy.setHeightForWidth(mChangeTransportPinButton->sizePolicy().hasHeightForWidth());
	mChangeTransportPinButton->setSizePolicy(transportPinSizePolicy);

	conclusionPageVLayout->addWidget(mChangeTransportPinButton);


	QLabel* conclusionDescLabel = new QLabel(tr("<br>AusweisApp2 is now ready for use. You can further configure AusweisApp2 via the \"Settings\" dialog from the navigation section. AusweisApp2 uses the proxy settings configured in your system. This setup assistant can be started at any time from the \"Help\" menu. The \"Finish\" button closes the setup assistant."));
	conclusionDescLabel->setWordWrap(true);
	conclusionDescLabel->setFocusPolicy(Qt::TabFocus);
	conclusionDescLabel->setAccessibleName(tr("Almost done! Step %1 of %2. AusweisApp2 is now ready for use. You can further configure AusweisApp2 via the Settings dialog from the navigation section. AusweisApp2 uses the proxy settings configured in your system. This setup assistant can be started at any time from the Help menu. The Finish button closes the setup assistant.").arg(this->pageIds().size() + 1).arg(mPageCount));

	GuiUtils::updateFontSize(conclusionDescLabel);

	conclusionPageVLayout->addWidget(conclusionDescLabel);

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	conclusionPageVLayout->addItem(verticalSpacer);

	return conclusionPage;
}


void SetupAssistantWizard::onChangeTransportPinButtonPressed()
{
	AppSettings::getInstance().getHistorySettings().setEnabled(mSaveHistoryCheckBox->isChecked());
	Q_EMIT fireChangePinButtonClicked();
	close();
}


void SetupAssistantWizard::accept()
{
	AppSettings::getInstance().getHistorySettings().setEnabled(mSaveHistoryCheckBox->isChecked());
	hide();
}


bool SetupAssistantWizard::eventFilter(QObject* /*pObject*/, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp(this->objectName());
			return true;
		}
	}
	return false;
}
