/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppQtMainWidget.h"

#include "AboutDialog.h"
#include "BuildHelper.h"
#include "GuiProfile.h"
#include "LogHandler.h"
#include "ReaderDetector.h"
#include "ReaderDriverDialog.h"
#include "SetupAssistantWizard.h"
#include "VersionNumber.h"
#include "generic/ExclusiveButtonGroup.h"
#include "generic/HelpAction.h"
#include "step/AuthenticateStepsWidget.h"
#include "ui_AppQtMainWidget.h"
#include "workflow/WorkflowQtWidget.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QIcon>
#include <QLoggingCategory>
#include <QMessageBox>
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
{
	mUi->setupUi(this);

	mUi->appLogoWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

	QStackedLayout* centralStackLayout = dynamic_cast<QStackedLayout*>(mUi->centralWidget->layout());
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

	//menu pin management
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

	connect(mUi->ausweisenPage, &SelfInformationWidget::selfAuthenticationRequested, this, &AppQtMainWidget::fireSelfAuthenticationRequested);
	connect(mUi->settingsPage, &SettingsWidget::changePinRequested, this, &AppQtMainWidget::fireChangePinRequested);
	connect(mUi->settingsPage, &SettingsWidget::diagnosisRequested, this, &AppQtMainWidget::fireDiagnosisRequested);
	connect(mUi->settingsPage, &SettingsWidget::driverRequested, this, &AppQtMainWidget::fireReaderDriverRequested);
	connect(mUi->settingsPage, &SettingsWidget::settingsDone, this, &AppQtMainWidget::onSettingsDone);

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
		{mUi->ausweisenToolButton, mUi->ausweisenPage, mUi->actionAusweisen, ":/images/bt_1.svg", ":/images/bt_1b.svg"},
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
		mUi->betaLabel->setMinimumSize(QSize(150, 150));
		mUi->betaLabel->setPixmap(QPixmap(QStringLiteral(":/images/beta.svg")).scaled(mUi->betaLabel->width(), mUi->betaLabel->width(), Qt::KeepAspectRatio));
		mUi->betaLabel->setVisible(true);
	}


	// work-around for bug QT/QTBUG-40869
	QSet<QObject*> visitedObjects;
	updateGeometryRecursively(this, visitedObjects);

#ifdef Q_OS_WIN
	// we need to call create() explicitly because Windows needs a handle to fire WM_ENDSESSION!
	// Since we start as a systemtray only we don't have a correct handle until we call show()
	create();
#endif
}


AppQtMainWidget::~AppQtMainWidget()
{
}


void AppQtMainWidget::changeEvent(QEvent* pEvent)
{
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
		if (QStackedWidget* stackedWidget = dynamic_cast<QStackedWidget*>(containerParent))
		{
			mSelectedPagesBeforeWorkflow.append(stackedWidget->currentWidget());
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
		if (QStackedWidget* stackedWidget = dynamic_cast<QStackedWidget*>(widget->parentWidget()))
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
		if (dynamic_cast<QStackedWidget*>(containerParent) != nullptr)
		{
			mSelectedPagesBeforeWorkflow += containingWidget;
		}

		containingWidget = containerParent;
	}

	mUi->settingsPage->switchToGuiModule(GuiModule::PIN_SETTINGS);
}


void AppQtMainWidget::closeEvent(QCloseEvent* pEvent)
{
	bool doClose = true;
	Q_EMIT fireCloseWindowRequested(&doClose);
	if (doClose)
	{
		pEvent->accept();
	}
	else
	{
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

#if defined(Q_OS_OSX)
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


void AppQtMainWidget::updateGeometryRecursively(QWidget* pWidget, QSet<QObject*>& pVisitedObjects)
{
	if (pVisitedObjects.contains(pWidget))
	{
		return;
	}

	pVisitedObjects.insert(pWidget);

	pWidget->updateGeometry();

	if (pWidget->layout() != nullptr)
	{
		updateGeometryRecursively(pWidget->layout(), pVisitedObjects);
	}

	for (QObject* child : pWidget->children())
	{
		if (QWidget* widget = dynamic_cast<QWidget*>(child))
		{
			updateGeometryRecursively(widget, pVisitedObjects);
		}
	}
}


void AppQtMainWidget::updateGeometryRecursively(QLayout* pLayout, QSet<QObject*>& pVisitedObjects)
{
	if (pVisitedObjects.contains(pLayout))
	{
		return;
	}

	pVisitedObjects.insert(pLayout);

	pLayout->invalidate();

	int itemCount = pLayout->count();
	for (int i = 0; i < itemCount; ++i)
	{
		QLayoutItem* item = pLayout->itemAt(i);

		if (QLayout* layout = item->layout())
		{
			updateGeometryRecursively(layout, pVisitedObjects);
		}
		else if (QWidget* widget = item->widget())
		{
			updateGeometryRecursively(widget, pVisitedObjects);
		}
	}
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
	QString filename = QFileDialog::getSaveFileName(this, QCoreApplication::applicationName() + " - " + tr("Save file"), QDir::homePath().append("/AusweisApp2.log"), QStringLiteral("*.log"));
	if (!filename.isNull())
	{
		if (!filename.endsWith(QStringLiteral(".log"), Qt::CaseSensitivity::CaseInsensitive))
		{
			filename.append(".log");
		}

		qCDebug(gui) << "File location:" << filename;

		if (QFile::exists(filename))
		{
			bool deleted = QFile::remove(filename);
			qCDebug(gui) << "Delete file location:" << deleted;
		}

		bool copied = LogHandler::getInstance().copy(filename);
		qCDebug(gui) << "Copy log to file location:" << copied;
	}
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
	if (QAction* action = dynamic_cast<QAction*>(sender()))
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
	QString link = tr("https://www.ausweisapp.bund.de/en/service/haeufig-gestellte-fragen/");
	QDesktopServices::openUrl(QUrl(link));
}


void AppQtMainWidget::onSendErrorActionClicked()
{
	QString link = tr("https://www.ausweisapp.bund.de/en/feedback/melden-sie-einen-fehler/");
	QDesktopServices::openUrl(QUrl(link));
}


void AppQtMainWidget::onEvaluateActionClicked()
{
	QString link = tr("https://www.ausweisapp.bund.de/en/feedback/bewerten-sie-uns/");
	QDesktopServices::openUrl(QUrl(link));
}


void AppQtMainWidget::onContentActionClicked()
{
	QString name = mUi->stackedWidget->widget(mUi->stackedWidget->currentIndex())->objectName();
	if (name.startsWith(QLatin1String("settingsPage")))
	{
		SettingsWidget* settingsWidget = static_cast<SettingsWidget*>(mUi->stackedWidget->widget(mUi->stackedWidget->currentIndex()));
		HelpAction::openContextHelp(settingsWidget->getActiveTabObjectName());
	}
	else
	{
		HelpAction::openContextHelp(name);
	}
}


void AppQtMainWidget::onAboutActionClicked()
{
	AboutDialog* dialog = new AboutDialog(this);
	dialog->show();
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


bool AppQtMainWidget::isRemindUserToClose()
{
	return AppSettings::getInstance().getGeneralSettings().isRemindUserToClose();
}
