/*!
 * AppQtGui.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "AppQtGui.h"

#include "AppSettings.h"
#include "DiagnosisGui.h"
#include "GuiProfile.h"
#include "NetworkManager.h"
#include "ReaderDriverGui.h"
#include "SetupAssistantGui.h"
#include "UpdateWindow.h"
#include "Updater.h"
#include "generic/HelpAction.h"
#include "generic/Page.h"
#include "workflow/WorkflowAuthenticateQtGui.h"
#include "workflow/WorkflowChangePinQtGui.h"
#include "workflow/WorkflowGui.h"
#include "workflow/WorkflowSelfInfoQtGui.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFile>
#include <QFileSystemWatcher>
#include <QHBoxLayout>
#include <QLoggingCategory>
#include <QMenu>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>


#if defined(Q_OS_OSX)
#import <Cocoa/Cocoa.h>
#endif


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


AppQtGui::AppQtGui()
	: QObject()
	, mMainWidget(nullptr)
	, mIcon(QStringLiteral(":/images/npa.svg"))
	, mTrayIcon(nullptr)
	, mActiveWorkflowUi()
	, mSetupAssistantGui(nullptr)
	, mDiagnosisGui(nullptr)
	, mReaderDriverGui(nullptr)
	, mAggressiveToForeground(false)
{
	initGuiProfile();

	mMainWidget = new AppQtMainWidget();
	mMainWidget->setWindowIcon(mIcon);
}


AppQtGui::~AppQtGui()
{
	if (mTrayIcon != nullptr)
	{
		QMenu* menu = mTrayIcon->contextMenu();
		if (menu != nullptr)
		{
			for (auto action : menu->actions())
			{
				delete action;
			}
			delete menu;
		}
	}

	delete mMainWidget;
}


void AppQtGui::init()
{
	connect(mMainWidget, &AppQtMainWidget::fireChangePinRequested, this, &AppQtGui::fireChangePinRequested);
	connect(mMainWidget, &AppQtMainWidget::fireSetupAssistantWizardRequest, this, &AppQtGui::onSetupAssistantWizardRequest);
	connect(mMainWidget, &AppQtMainWidget::fireDiagnosisRequested, this, &AppQtGui::onDiagnosisRequested);
	connect(mMainWidget, &AppQtMainWidget::fireReaderDriverRequested, this, &AppQtGui::onReaderDriverRequested);
	connect(mMainWidget, &AppQtMainWidget::fireCloseWindowRequested, this, &AppQtGui::onCloseWindowRequested);
	connect(mMainWidget, &AppQtMainWidget::fireSelfAuthenticationRequested, this, &AppQtGui::selfAuthenticationRequested);
	connect(mMainWidget, &AppQtMainWidget::fireQuitApplicationRequested, this, &AppQtGui::quitApplicationRequested);
	connect(mMainWidget, &AppQtMainWidget::fireChangeHighContrast, this, &AppQtGui::onChangeHighContrast);
}


void AppQtGui::onApplicationStarted()
{
	if (QSystemTrayIcon::isSystemTrayAvailable())
	{
		createTrayIcon();
	}

	if (mTrayIcon != nullptr)
	{
		mTrayIcon->show();
		mTrayIcon->showMessage(QLatin1String(""), tr("AusweisApp2 was started."), QSystemTrayIcon::NoIcon, 3000);
	}

	if (!QSystemTrayIcon::isSystemTrayAvailable()
			|| GuiProfile::getProfile().getShowWindow()
			|| AppSettings::getInstance().getGeneralSettings().isShowSetupAssistant()
			|| AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
	{
		QMetaObject::invokeMethod(this, "show", Qt::QueuedConnection);
	}

	if (AppSettings::getInstance().getGeneralSettings().isShowSetupAssistant())
	{
		mMainWidget->setSelectedTab(nullptr);
		QMetaObject::invokeMethod(this, "onSetupAssistantWizardRequest", Qt::QueuedConnection);

		// just show the setup assistant once
		AppSettings::getInstance().getGeneralSettings().setShowSetupAssistant(false);
		AppSettings::getInstance().getGeneralSettings().save();
	}

	if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
	{
		QMetaObject::invokeMethod(this, "onDeveloperModeQuestion", Qt::QueuedConnection);
	}
}


QSharedPointer<WorkflowAuthenticateQtGui> AppQtGui::createWorkflowAuthenticateUi(const QSharedPointer<AuthContext>& pContext)
{
	return QSharedPointer<WorkflowAuthenticateQtGui>(new WorkflowAuthenticateQtGui(pContext, mMainWidget));
}


QSharedPointer<WorkflowChangePinQtGui> AppQtGui::createWorkflowChangePinUi(const QSharedPointer<ChangePinContext>& pContext)
{
	return QSharedPointer<WorkflowChangePinQtGui>(new WorkflowChangePinQtGui(pContext, mMainWidget));
}


QSharedPointer<WorkflowSelfInfoQtGui> AppQtGui::createWorkflowSelfInfoUi(const QSharedPointer<SelfAuthenticationContext>& pContext)
{
	return QSharedPointer<WorkflowSelfInfoQtGui>(new WorkflowSelfInfoQtGui(pContext, mMainWidget));
}


void AppQtGui::activateWorkflowUi(QSharedPointer<WorkflowGui> pWorkflowUi, bool pAllowHideAfterWorkflow)
{
	if (pWorkflowUi)
	{
		mActiveWorkflowUi = pWorkflowUi;
		mActiveWorkflowUi->activate();
	}

	mMainWidget->activateMenuBarItems(false);
	closeDialogs();

#ifdef Q_OS_WIN
	mAggressiveToForeground = mActiveWorkflowUi.objectCast<WorkflowAuthenticateQtGui>();
#endif
	bool hideAfterWorkflow = pAllowHideAfterWorkflow
			&& mActiveWorkflowUi.objectCast<WorkflowAuthenticateQtGui>()
			&& AppSettings::getInstance().getGeneralSettings().isAutoCloseWindowAfterAuthentication();
	mMainWidget->setHideWindowAfterWorkflow(hideAfterWorkflow);
	show();
}


void AppQtGui::deactivateCurrentWorkflowUi()
{
	if (mMainWidget->isHideWindowAfterWorkflow())
	{
		hideFromTaskbar();
		mMainWidget->hide();
	}
	mMainWidget->activateMenuBarItems(true);

	if (mActiveWorkflowUi)
	{
		mActiveWorkflowUi->deactivate();
		mActiveWorkflowUi.clear();
	}
}


void AppQtGui::onShowUserInformation(const QString& pInformationMessage)
{
	QMessageBox msgBox(mMainWidget);
	msgBox.setWindowTitle(QCoreApplication::applicationName() + " - Information");
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText(pInformationMessage);
	msgBox.setStandardButtons(QMessageBox::StandardButton::Ok);
	msgBox.exec();
}


void AppQtGui::onSetupAssistantWizardRequest()
{
	if (!mSetupAssistantGui)
	{
		mSetupAssistantGui = new SetupAssistantGui(mMainWidget);
		connect(mSetupAssistantGui, &SetupAssistantGui::fireChangePinButtonClicked, mMainWidget, &AppQtMainWidget::onChangePinButtonClicked);
	}
	mSetupAssistantGui->activate();
}


void AppQtGui::onDeveloperModeQuestion()
{
	QMessageBox msgBox(mMainWidget);
	msgBox.setWindowModality(Qt::WindowModal);
	msgBox.setWindowFlags(msgBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	msgBox.setIconPixmap(mMainWidget->windowIcon().pixmap(QSize(48, 48)));
	msgBox.setText(tr("The developer mode is enabled."));
	msgBox.setInformativeText(tr("Do you want to disable the developer mode?"));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);

	if (msgBox.exec() == QMessageBox::Yes)
	{
		AppSettings::getInstance().getGeneralSettings().setDeveloperMode(false);
		AppSettings::getInstance().getGeneralSettings().save();
	}
}


void AppQtGui::onDiagnosisRequested()
{
	if (!mDiagnosisGui)
	{
		mDiagnosisGui = new DiagnosisGui(mMainWidget);
	}
	mDiagnosisGui->activate();
}


void AppQtGui::onReaderDriverRequested()
{
	if (!mReaderDriverGui)
	{
		mReaderDriverGui = new ReaderDriverGui(mMainWidget);
	}
	mReaderDriverGui->activate();
}


bool AppQtGui::askChangeTransportPinNow()
{
	GeneralSettings& settings = AppSettings::getInstance().getGeneralSettings();
	if (!settings.isTransportPinReminder())
	{
		return false;
	}
	settings.setTransportPinReminder(false);
	settings.save();

	show(UiModule::PINMANAGEMENT);
	closeDialogs();

	QMessageBox messageBox(mMainWidget);
	messageBox.setWindowTitle(QCoreApplication::applicationName() + " - Information");
	messageBox.setWindowModality(Qt::WindowModal);
	messageBox.setWindowFlags(messageBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	messageBox.setText(tr("Did you change the initial transport PIN already?<br><br>Prior to the first use of the online identification function you have to replace the transport PIN by an individual 6-digit PIN. Online identification with transport PIN is not possible."));
	messageBox.setStandardButtons(QMessageBox::Yes);
	auto changePinButton = messageBox.addButton(tr("No, change transport PIN now"), QMessageBox::NoRole);
	messageBox.exec();

	if (messageBox.clickedButton() != changePinButton)
	{
		return false;
	}

	return true;
}


bool AppQtGui::eventFilter(QObject* /*pObject*/, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp();
			return true;
		}

		if (keyEvent->key() == Qt::BackButton || keyEvent->key() == Qt::Key_Back)
		{
			Q_EMIT quitApplicationRequested();
			return true;
		}
	}
	return false;
}


void AppQtGui::initGuiProfile()
{
	GuiProfile& profile = GuiProfile::getProfile();

	QSize maxWindowSize = profile.getWindowSize();
	if (!maxWindowSize.isValid())
	{
		QDesktopWidget widget;
		maxWindowSize = widget.screenGeometry(widget.primaryScreen()).size();
	}

	qCDebug(gui) << "max_window_size:" << maxWindowSize;
	bool smallScreen = static_cast<qint64>(maxWindowSize.width()) * static_cast<qint64>(maxWindowSize.height()) < 700000;
	if (smallScreen)
	{
		profile.setScreenSize(ScreenSize::SMALL);
	}
	else
	{
		profile.setScreenSize(ScreenSize::MEDIUM);
	}

	profile.setShowMainTabs(!smallScreen);

	// set style sheet only if none has be specified on the command line
	if (qApp->styleSheet().isEmpty())
	{
		if (profile.getDebugStyleSheet().isEmpty())
		{
			loadStyleSheet(profile.getStyleSheetName(), true);
		}
		else
		{
			QFileSystemWatcher* watcher = new QFileSystemWatcher(QStringList() << profile.getDebugStyleSheet(), this);
			connect(watcher, &QFileSystemWatcher::fileChanged, this, &AppQtGui::onDebugStyleSheetChanged);
			loadStyleSheet(profile.getDebugStyleSheet(), false);
		}
	}
}


void AppQtGui::loadStyleSheet(const QString& pStyleSheetName, bool pIsResource)
{
	QString styleSheetName = pIsResource ? QStringLiteral(":/stylesheets/") + pStyleSheetName : pStyleSheetName;
	qCDebug(gui) << "loading style sheet" << styleSheetName;
	QString styleSheet = readStyleSheet(styleSheetName);
	if (!styleSheet.isEmpty())
	{
		qApp->setStyleSheet(styleSheet);
	}
	else
	{
		qCWarning(gui) << "Failed to load global style sheet!";
	}
}


QString AppQtGui::readStyleSheet(const QString& pFileName)
{
	// read the file into a string
	QFile file(pFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		qCWarning(gui) << "Failed to read style sheet:" << pFileName;
		return QString();
	}

	QString styleSheet = QString::fromLatin1(file.readAll());
	file.close();

	// resolve imports
	// Note: The algorithm is very simple and e.g. doesn't detect that an import is commented out.
	QRegularExpression regExp(QStringLiteral("@import\\s+\"([^\"]*)\"\\s*;"));
	QRegularExpressionMatchIterator it = regExp.globalMatch(styleSheet);
	if (!it.hasNext())
	{
		return styleSheet;
	}

	QString result;
	int lastOffset = 0;

	while (it.hasNext())
	{
		QRegularExpressionMatch match = it.next();
		if (lastOffset < match.capturedStart())
		{
			result += styleSheet.midRef(lastOffset, match.capturedStart() - lastOffset);
		}

		result += readStyleSheet(match.captured(1));

		lastOffset = match.capturedEnd();
	}

	if (lastOffset < styleSheet.length())
	{
		result += styleSheet.midRef(lastOffset);
	}

	return result;
}


void AppQtGui::onChangeHighContrast(bool* pHighContrastOn)
{
	if (*pHighContrastOn)
	{
		qApp->setStyleSheet(QString());
	}
	else
	{
		initGuiProfile();
	}
}


void AppQtGui::createTrayIcon()
{
	QMenu* trayIconMenu = new QMenu(nullptr);

#if defined(Q_OS_OSX)
	QAction* showApplicationAction = new QAction(tr("Open"), trayIconMenu);
	connect(showApplicationAction, &QAction::triggered, this, [this] {AppQtGui::show();
			}
			);
#endif

	QAction* quitAction = new QAction(tr("Exit AusweisApp2"), trayIconMenu);
	connect(quitAction, &QAction::triggered, this, &AppQtGui::quitApplicationRequested);

#if defined(Q_OS_OSX)
	trayIconMenu->addAction(showApplicationAction);
	trayIconMenu->addSeparator();
#endif
	trayIconMenu->addAction(quitAction);

	mTrayIcon = new QSystemTrayIcon(mIcon, mMainWidget);
	connect(mTrayIcon, &QSystemTrayIcon::activated, this, &AppQtGui::onActivated);
	connect(mTrayIcon, &QSystemTrayIcon::messageClicked, this, [this] {show(UiModule::CURRENT);
			});

	mTrayIcon->setContextMenu(trayIconMenu);
	mTrayIcon->setToolTip(QCoreApplication::applicationName());
}


void AppQtGui::closeDialogs()
{
	if (mSetupAssistantGui)
	{
		mSetupAssistantGui->deactivate();
	}
	if (mDiagnosisGui)
	{
		mDiagnosisGui->deactivate();
	}
	if (mReaderDriverGui)
	{
		mReaderDriverGui->deactivate();
	}
}


void AppQtGui::hideFromTaskbar()
{
#if defined(Q_OS_OSX)
	ProcessSerialNumber psn = {
		0, kCurrentProcess
	};
	TransformProcessType(&psn, kProcessTransformToBackgroundApplication);
#endif
}


void AppQtGui::restoreToTaskbar()
{
#if defined(Q_OS_OSX)
	ProcessSerialNumber psn = {
		0, kCurrentProcess
	};
	TransformProcessType(&psn, kProcessTransformToForegroundApplication);
	[NSApp activateIgnoringOtherApps: YES];
#endif
}


void AppQtGui::onActivated(QSystemTrayIcon::ActivationReason pReason)
{
#ifdef Q_OS_OSX
	Q_UNUSED(pReason)
#else
	if (pReason == QSystemTrayIcon::Trigger)
	{
		show();
	}
#endif
}


void AppQtGui::onCloseWindowRequested(bool* pDoClose)
{
	if (mMainWidget->isRemindUserToClose())
	{
		QMessageBox messageBox(mMainWidget);
		messageBox.installEventFilter(this);
		messageBox.setWindowTitle(QApplication::applicationName() + " - Information");
		messageBox.setWindowModality(Qt::WindowModal);
		messageBox.setIcon(QMessageBox::Information);
		messageBox.setWindowFlags(messageBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
		messageBox.setText(tr("The user interface of the %1 is closed.").arg(QApplication::applicationName()));
		messageBox.setInformativeText(tr("The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.").arg(QApplication::applicationName()));
		messageBox.setCheckBox(new QCheckBox(tr("Don't show this dialog again.")));
		messageBox.exec();

		Q_EMIT fireCloseReminderFinished(messageBox.checkBox()->isChecked());
	}

	if (mActiveWorkflowUi == nullptr)
	{
		*pDoClose = true;
	}
	else if ((*pDoClose = mActiveWorkflowUi->verifyAbortWorkflow()))
	{
		Q_EMIT mActiveWorkflowUi->fireUserCancelled();
	}

	if (*pDoClose)
	{
		hideFromTaskbar();
	}
}


void AppQtGui::onDebugStyleSheetChanged(const QString& pPath)
{
	if (QFileSystemWatcher* watcher = qobject_cast<QFileSystemWatcher*>(sender()))
	{
		// work-around for QFileSystemWatcher no longer knowing the file after receiving the first notification
		watcher->removePath(pPath);
		watcher->addPath(pPath);
	}

	loadStyleSheet(pPath, false);
}


#ifndef QT_NO_NETWORKPROXY
void AppQtGui::onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
{
	CredentialDialog dialog(mMainWidget);
	dialog.setUser(pProxy.user());

	if (dialog.exec() == QDialog::Accepted)
	{
		pAuthenticator->setUser(dialog.getUser());
		pAuthenticator->setPassword(dialog.getPassword());
	}
}


#endif


void AppQtGui::show(UiModule pModule)
{
	if (!mActiveWorkflowUi.isNull())
	{
		pModule = UiModule::CURRENT;
	}

	switch (pModule)
	{
		case UiModule::PINMANAGEMENT:
			mMainWidget->switchToGuiModule(GuiModule::PIN_SETTINGS);
			break;

		case UiModule::SETTINGS:
			mMainWidget->switchToGuiModule(GuiModule::GENERAL_SETTINGS);
			break;

		case UiModule::DEFAULT:
			mMainWidget->switchToGuiModule(GuiModule::START_PAGE);
			break;

		case UiModule::CURRENT:
			// don't switch the module, just show the current one
			break;
	}

	restoreToTaskbar();

	if (mMainWidget->isMinimized())
	{
		mMainWidget->showNormal();
	}

	// Ensure the window's minimumSizeHint is respected (work-around for a Windows Qt bug).
	if (!mMainWidget->isMaximized())
	{
		QSize size = mMainWidget->size();
		size = size.expandedTo(mMainWidget->minimumSizeHint());
		mMainWidget->resize(size);
	}

#ifdef Q_OS_WIN
	if (mAggressiveToForeground)
	{
		// Changing the window flags seems to be the only way to
		// bring the window to the foreground if it is not minimized.
		const Qt::WindowFlags flags = mMainWidget->windowFlags();
		mMainWidget->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
		mMainWidget->show();
		mMainWidget->setWindowFlags(flags);

		mAggressiveToForeground = false;
	}
#endif

	mMainWidget->show();
	mMainWidget->activateWindow();

	// Work-around for the window not being brought to the foreground. Invoke
	// activateWindow() again after the events currently pending in the event
	// queue have been processed. This appears to do the job. Note that the
	// first activateWindow() above is apparently still necessary.
	QCoreApplication::processEvents();
	mMainWidget->activateWindow();

	static bool firstCheck = true;
	if (firstCheck)
	{
		firstCheck = false;
		Updater::getInstance().update();
		if (AppSettings::getInstance().getGeneralSettings().isAutoUpdateCheck())
		{
			new UpdateWindow(true, mMainWidget);
		}
	}
}


void AppQtGui::shutdown()
{
	if (mTrayIcon != nullptr)
	{
		mTrayIcon->hide();
	}
}
