/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "AppQtMainWidget.h"
#include "ui_AppQtMainWidget.h"

#include "AboutDialog.h"
#include "AppSettings.h"
#include "BuildHelper.h"
#include "generic/ExclusiveButtonGroup.h"
#include "HelpAction.h"
#include "LanguageLoader.h"
#include "LogHandler.h"
#include "ReaderDetector.h"
#include "step/AuthenticateStepsWidget.h"
#include "SetupAssistantWizard.h"
#include "VersionNumber.h"
#include "workflow/WorkflowQtWidget.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QIcon>
#include <QLoggingCategory>
#include <QStackedLayout>
#include <QTimer>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(gui)

AppQtMainWidget::AppQtMainWidget()
	: QMainWindow()
	, mUi(new Ui::AppQtMainWidget())
	, mTabButton2Page()
	, mTabAction2Button()
	, mAuthenticationWorkflowWidget(nullptr)
	, mSelectedPushButton(nullptr)
	, mSelectedPushButtonBeforeWorkflow(nullptr)
	, mSelectedPagesBeforeWorkflow()
	, mHideWindowAfterWorkflow(false)
	, mLogFilesDialog()
	, mClosingDialogsPending(false)
	, mCloseWithoutConfirmation(false)
{
	mUi->setupUi(this);
	mUi->englishButton->setBackgroundRole(QPalette::NoRole);
	mUi->germanButton->setBackgroundRole(QPalette::NoRole);

	refreshLanguageButton();

	mUi->appLogoWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

	auto* centralStackLayout = qobject_cast<QStackedLayout*>(mUi->centralWidget->layout());
	centralStackLayout->setStackingMode(QStackedLayout::StackAll);
	centralStackLayout->setCurrentIndex(1);

	QSpacerItem* spacer = new QSpacerItem(1, mUi->logoLabel->sizeHint().height(), QSizePolicy::Fixed, QSizePolicy::Ignored);
	mUi->applicationGridLayout->addItem(spacer, mUi->applicationGridLayout->rowCount(), 0);

	mUi->providerPage->layout()->setMargin(20);
	mUi->historyPage->layout()->setMargin(20);

	mAuthenticationWorkflowWidget = new WorkflowQtWidget();
	mUi->workflowPage->layout()->addWidget(mAuthenticationWorkflowWidget);
	mAuthenticationWorkflowWidget->getStepWidgetArea()->layout()->addWidget(new AuthenticateStepsWidget);

	//menu file
	connect(mUi->actionBeenden, &QAction::triggered, this, &AppQtMainWidget::fireQuitApplicationRequested);
	connect(mUi->actionSettings, &QAction::triggered, this, &AppQtMainWidget::onSettingsButtonClicked);

	//menu PIN management
	connect(mUi->actionChangePin, &QAction::triggered, this, &AppQtMainWidget::onChangePinButtonClicked);

	//help
	connect(mUi->actionSetupAssistant, &QAction::triggered, this, &AppQtMainWidget::fireSetupAssistantWizardRequest);
	connect(mUi->actionDiagnosis, &QAction::triggered, this, &AppQtMainWidget::fireDiagnosisRequested);

	connect(mUi->actionShowProtocol, &QAction::triggered, this, &AppQtMainWidget::onOpenLoggingFileButtonClicked);
	connect(mUi->actionSaveProtocol, &QAction::triggered, this, &AppQtMainWidget::onSaveLoggingFileButtonClicked);
	connect(mUi->actionInfo, &QAction::triggered, this, &AppQtMainWidget::onAboutActionClicked);
	connect(mUi->actionSendError, &QAction::triggered, this, &AppQtMainWidget::onSendErrorActionClicked);
	connect(mUi->actionEvaluate, &QAction::triggered, this, &AppQtMainWidget::onEvaluateActionClicked);
	connect(mUi->actionQuestion, &QAction::triggered, this, &AppQtMainWidget::onQuestionActionClicked);
	connect(mUi->actionHelp, &QAction::triggered, this, &AppQtMainWidget::onContentActionClicked);

	connect(mUi->identifyPage, &SelfInformationWidget::selfAuthenticationRequested, this, &AppQtMainWidget::fireSelfAuthenticationRequested);
	connect(mUi->settingsPage, &SettingsWidget::changePinRequested, this, &AppQtMainWidget::fireChangePinRequested);
	connect(mUi->settingsPage, &SettingsWidget::diagnosisRequested, this, &AppQtMainWidget::fireDiagnosisRequested);
	connect(mUi->settingsPage, &SettingsWidget::settingsDone, this, &AppQtMainWidget::onSettingsDone);
	connect(mUi->settingsPage, &SettingsWidget::fireSwitchUiRequested, this, &AppQtMainWidget::fireSwitchUiRequested);

	connect(mUi->germanButton, &QPushButton::clicked, this, [&]()
			{
				setLanguage(QLocale::German);
			});
	connect(mUi->englishButton, &QPushButton::clicked, this, [&]()
			{
				setLanguage(QLocale::English);
			});

	//NavigationButtons
	mUi->startPushButton->setVisible(false);
	mUi->mainTabList->addButton(mUi->startPushButton);
	mTabButton2Page.insert(mUi->startPushButton, mUi->applicationPage);

	struct ButtonInfo
	{
		QAbstractButton* mButton;
		QWidget* mPage;
		QAction* mAction;
		const char* mOnIcon;
		const char* mOffIcon;
	};
	ButtonInfo buttonInfos[] = {
		{mUi->ausweisenToolButton, mUi->identifyPage, mUi->actionAusweisen, ":/images/bt_1.svg", ":/images/bt_1b.svg"},
		{mUi->providerToolButton, mUi->providerPage, mUi->actionProvider, ":/images/bt_2.svg", ":/images/bt_2b.svg"},
		{mUi->historyToolButton, mUi->historyPage, mUi->actionHistory, ":/images/bt_3.svg", ":/images/bt_3b.svg"},
		{mUi->settingsToolButton, mUi->settingsPage, mUi->actionSettings, ":/images/bt_4.svg", ":/images/bt_4b.svg"},
	};

	for (size_t i = 0; i < sizeof(buttonInfos) / sizeof(buttonInfos[0]); ++i)
	{
		const ButtonInfo& buttonInfo = buttonInfos[i];
		QIcon icon;
		icon.addPixmap(QPixmap(QString::fromLatin1(buttonInfo.mOnIcon)), QIcon::Normal, QIcon::Off);
		icon.addPixmap(QPixmap(QString::fromLatin1(buttonInfo.mOffIcon)), QIcon::Normal, QIcon::On);

		buttonInfo.mButton->setIcon(icon);

		connect(buttonInfo.mAction, &QAction::triggered, this, &AppQtMainWidget::onTabActionTriggered);

		mUi->mainTabList->addButton(buttonInfo.mButton);
		mTabButton2Page.insert(buttonInfo.mButton, buttonInfo.mPage);
		mTabAction2Button.insert(buttonInfo.mAction, buttonInfo.mButton);
	}

	connect(mUi->mainTabList, &TabButtonGroup::buttonToggled, this, &AppQtMainWidget::onTabButtonToggled);
	mUi->stackedWidget->setCurrentIndex(0);

	if (VersionNumber::getApplicationVersion().isDeveloperVersion())
	{
		setWindowTitle(windowTitle() + QStringLiteral(" - Beta - ") + QCoreApplication::applicationVersion());
		mUi->betaLabel->setVisible(true);
	}
	else
	{
		mUi->betaLabel->setVisible(false);
	}

#ifdef Q_OS_WIN
	// we need to call create() explicitly because Windows needs a handle to fire WM_ENDSESSION!
	// Since we start as a systemtray only we don't have a correct handle until we call show()
	create();
#endif

	connect(this, &AppQtMainWidget::fireAskUserToConfirmClosing, this, &AppQtMainWidget::onAskUserToConfirmClosing, Qt::QueuedConnection);
}


AppQtMainWidget::~AppQtMainWidget()
{
}


void AppQtMainWidget::showEvent(QShowEvent* pEvent)
{
	Q_UNUSED(pEvent)
	mCloseWithoutConfirmation = false;
}


void AppQtMainWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
		refreshLanguageButton();
	}

	if (pEvent->type() == QEvent::PaletteChange) // QEvent::StyleChange is called, too
	{
#if defined(Q_OS_WIN)
		HIGHCONTRAST hc;
		hc.cbSize = sizeof(hc);

		bool highContrastOn = false;
		if (SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, FALSE) && (hc.dwFlags & HCF_HIGHCONTRASTON))
		{
			qDebug() << "High contrast (SPI_GETHIGHCONTRAST) switched on.";
			highContrastOn = true;
			Q_EMIT fireChangeHighContrast(&highContrastOn);
		}
		else
		{
			qDebug() << "High contrast (SPI_GETHIGHCONTRAST) switched off.";
			Q_EMIT fireChangeHighContrast(&highContrastOn);
		}
#endif
	}
	QWidget::changeEvent(pEvent);
}


bool AppQtMainWidget::remoteScanRunning() const
{
	return mUi->settingsPage->remoteScanRunning();
}


void AppQtMainWidget::workflowActivated(WorkflowWidgetParent pParent, const QString& /*pName*/)
{
	QAbstractButton* tabToolButton = nullptr;
	QWidget* containingWidget = nullptr;

	// activate the correct tab and set the workflow page
	switch (pParent)
	{
		case WorkflowWidgetParent::SelfAuthentication:
		case WorkflowWidgetParent::Authentication:
			tabToolButton = mUi->ausweisenToolButton;
			containingWidget = mUi->workflowPage;
			break;

		case WorkflowWidgetParent::SettingsChangePin:
			tabToolButton = mUi->settingsToolButton;
			containingWidget = nullptr;
			break;
	}

	mSelectedPushButtonBeforeWorkflow = mSelectedPushButton;

	setSelectedTab(tabToolButton);

	if (tabToolButton != nullptr)
	{
		const auto tabButtons = mTabButton2Page.keys();
		for (auto button : tabButtons)
		{
			if (button != tabToolButton)
			{
				button->setEnabled(false);
			}
		}
	}

	// show the respective page in the widget stacks
	while (containingWidget != nullptr)
	{
		QWidget* containerParent = containingWidget->parentWidget();
		if (auto* stackedWidget = qobject_cast<QStackedWidget*>(containerParent))
		{
			mSelectedPagesBeforeWorkflow += stackedWidget->currentWidget();
			stackedWidget->setCurrentWidget(containingWidget);
		}

		containingWidget = containerParent;
	}

	mUi->settingsPage->workflowStarted();
	mUi->mainTabList->setWorkflowActive(true);
}


void AppQtMainWidget::workflowDeactivated()
{
	const auto tabButtons = mTabButton2Page.keys();
	for (auto button : tabButtons)
	{
		button->setEnabled(true);
	}

	for (auto widget : qAsConst(mSelectedPagesBeforeWorkflow))
	{
		if (auto* stackedWidget = qobject_cast<QStackedWidget*>(widget->parentWidget()))
		{
			stackedWidget->setCurrentWidget(widget);
		}
	}
	mSelectedPagesBeforeWorkflow.clear();

	mUi->settingsPage->workflowFinished();
	mUi->mainTabList->setWorkflowActive(false);

	// switch back to the tab selected before the workflow
	if (mSelectedPushButtonBeforeWorkflow == nullptr)
	{
		return;
	}
	setSelectedTab(mSelectedPushButtonBeforeWorkflow);
	mSelectedPushButtonBeforeWorkflow = nullptr;
}


void AppQtMainWidget::switchToGuiModule(GuiModule pModule)
{
	switch (pModule)
	{
		case GuiModule::START_PAGE:
			setSelectedTab(mUi->startPushButton);
			break;

		case GuiModule::IDENTIFY:
			setSelectedTab(mUi->ausweisenToolButton);
			break;

		case GuiModule::GENERAL_SETTINGS:
		case GuiModule::PIN_SETTINGS:
			setSelectedTab(mUi->settingsToolButton);
			mUi->settingsPage->switchToGuiModule(pModule);
			break;
	}
}


void AppQtMainWidget::switchToPinSettingsAfterWorkflow()
{
	mHideWindowAfterWorkflow = false;
	mSelectedPushButtonBeforeWorkflow = mUi->settingsToolButton;

	mSelectedPagesBeforeWorkflow.clear();

	QWidget* containingWidget = mUi->stackedWidget->parentWidget();

	while (containingWidget != nullptr)
	{
		QWidget* containerParent = containingWidget->parentWidget();
		if (qobject_cast<QStackedWidget*>(containerParent) != nullptr)
		{
			mSelectedPagesBeforeWorkflow += containingWidget;
		}

		containingWidget = containerParent;
	}

	mUi->settingsPage->switchToGuiModule(GuiModule::PIN_SETTINGS);
}


void AppQtMainWidget::hideWithoutConfirmation()
{
	if (mLogFilesDialog)
	{
		mLogFilesDialog->closeActiveDialogs();
		mLogFilesDialog->reject();
	}

	Q_EMIT fireCloseActiveDialogs();
	mCloseWithoutConfirmation = true;
	close();
}


void AppQtMainWidget::onAskUserToConfirmClosing()
{
	bool doClose = true;
	Q_EMIT fireCloseWindowRequested(&doClose);
	if (doClose)
	{
		mCloseWithoutConfirmation = true;
		close();
	}
	else
	{
		mClosingDialogsPending = false;
	}
}


void AppQtMainWidget::closeEvent(QCloseEvent* pEvent)
{
	if (mCloseWithoutConfirmation)
	{
		pEvent->accept();
		mClosingDialogsPending = false;
	}
	else
	{
		if (!mClosingDialogsPending)
		{
			mClosingDialogsPending = true;
			Q_EMIT fireAskUserToConfirmClosing();
		}
		pEvent->ignore();
	}
}


void AppQtMainWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	switch (keyEvent->key())
	{
		case Qt::Key_F1:
			onContentActionClicked();
			break;

		default:
			break;
	}
}


void AppQtMainWidget::setSelectedTab(QAbstractButton* pSelectedPushButton)
{
	if (pSelectedPushButton == nullptr)
	{
		pSelectedPushButton = mUi->settingsToolButton;
		mUi->settingsPage->switchToGuiModule(GuiModule::GENERAL_SETTINGS);
	}
	if (mSelectedPushButton == mUi->settingsToolButton && mUi->settingsPage->isSettingsChanged())
	{
		const auto tabButtons = mTabButton2Page.keys();
		for (auto button : tabButtons)
		{
			button->setChecked(button == mUi->settingsToolButton);
		}
		mUi->settingsPage->showSettingsChangedMessage();
	}

	mUi->appLogoWidget->setVisible(pSelectedPushButton == mUi->startPushButton);

	if (mSelectedPushButton)
	{
		mSelectedPushButton->clearFocus();
	}
	mSelectedPushButton = pSelectedPushButton;
	mSelectedPushButton->setChecked(true);

	mUi->stackedWidget->setCurrentWidget(mTabButton2Page.value(pSelectedPushButton));
}


void AppQtMainWidget::activateWindow()
{
	QMainWindow::activateWindow();

#if defined(Q_OS_MACOS)
	// Workaround. When switching from "BackgroundApplication" to "ForegroundApplication"
	// on MacOS, it is a known problem that the menu bar of the previous active application
	// stays visible, although the window has changed to the current application. As soon
	// as the user clicks the menu, it magically transforms to the correct one. We therefore
	// manually trigger an update. Neither update() nor repaint() of QMenuBar solve this problem.
	QMenu menu;
	mUi->menuBar->addAction(menu.menuAction());
	mUi->menuBar->removeAction(menu.menuAction());
#endif
}


void AppQtMainWidget::onSettingsDone()
{
	setSelectedTab(mUi->startPushButton);
}


void AppQtMainWidget::onOpenLoggingFileButtonClicked()
{
	if (!mLogFilesDialog)
	{
		mLogFilesDialog = new LogFilesDialog(this);
	}

	mLogFilesDialog->show();
}


void AppQtMainWidget::onSaveLoggingFileButtonClicked()
{
	if (!mLogFilesDialog)
	{
		mLogFilesDialog = new LogFilesDialog(this);
	}

	mLogFilesDialog->saveLogFile(this);
}


void AppQtMainWidget::onTabButtonToggled(QAbstractButton* pButton, bool pChecked)
{
	if (pChecked)
	{
		if (mTabButton2Page.contains(pButton) && mSelectedPushButton != pButton)
		{
			setSelectedTab(pButton);
		}
	}
}


void AppQtMainWidget::onTabActionTriggered()
{
	if (auto* action = qobject_cast<QAction*>(sender()))
	{
		if (QAbstractButton* button = mTabAction2Button.value(action))
		{
			setSelectedTab(button);
		}
	}
}


void AppQtMainWidget::onSettingsButtonClicked()
{
	setSelectedTab(mUi->settingsToolButton);
	mUi->settingsPage->switchToGuiModule(GuiModule::GENERAL_SETTINGS);
}


void AppQtMainWidget::onChangePinButtonClicked()
{
	setSelectedTab(mUi->settingsToolButton);
	mUi->settingsPage->switchToGuiModule(GuiModule::PIN_SETTINGS);
}


void AppQtMainWidget::onQuestionActionClicked()
{
	QString link = tr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/");
	QDesktopServices::openUrl(QUrl(link));
}


void AppQtMainWidget::onSendErrorActionClicked()
{
	QString link = tr("https://www.ausweisapp.bund.de/en/qa/report-an-error/");
	QDesktopServices::openUrl(QUrl(link));
}


void AppQtMainWidget::onEvaluateActionClicked()
{
	QString link = tr("https://www.ausweisapp.bund.de/en/qa/evaluate-us/");
	QDesktopServices::openUrl(QUrl(link));
}


void AppQtMainWidget::onContentActionClicked()
{
	QString name = mUi->stackedWidget->widget(mUi->stackedWidget->currentIndex())->objectName();
	if (name.startsWith(QLatin1String("settingsPage")))
	{
		auto* settingsWidget = static_cast<SettingsWidget*>(mUi->stackedWidget->widget(mUi->stackedWidget->currentIndex()));
		HelpAction::openContextHelp(settingsWidget->getActiveTabObjectName());
	}
	else
	{
		HelpAction::openContextHelp(name);
	}
}


void AppQtMainWidget::onAboutActionClicked()
{
	auto* dialog = new AboutDialog(this);
	dialog->show();
}


void AppQtMainWidget::setLanguage(QLocale::Language pLocale)
{
	GeneralSettings& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	generalSettings.setLanguage(pLocale);
	generalSettings.save();
}


void AppQtMainWidget::refreshLanguageButton()
{
	const QString& selected = QStringLiteral("border-style: solid; border-width: 1px; border-color: grey; padding: 4px;");
	const QString& unselected = QStringLiteral("padding: 4px;");

	const auto& locale = LanguageLoader::getInstance().getUsedLocale();
	if (locale == QLocale::English)
	{
		mUi->englishButton->setStyleSheet(selected);
		mUi->germanButton->setStyleSheet(unselected);
	}
	else if (locale == QLocale::German)
	{
		mUi->englishButton->setStyleSheet(unselected);
		mUi->germanButton->setStyleSheet(selected);
	}
}


void AppQtMainWidget::activateMenuBarItems(bool pEnable)
{
	mUi->actionAusweisen->setEnabled(pEnable);
	mUi->actionProvider->setEnabled(pEnable);
	mUi->actionHistory->setEnabled(pEnable);
	mUi->actionSettings->setEnabled(pEnable);
	mUi->actionChangePin->setEnabled(pEnable);
	mUi->actionSetupAssistant->setEnabled(pEnable);
}


void AppQtMainWidget::onCloseWindowRequested()
{
	mUi->settingsPage->onTabChanged(-1);
}


bool AppQtMainWidget::isRemindUserToClose()
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isRemindUserToClose();
}
