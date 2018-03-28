/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "AppQtGui.h"

#include "AppSettings.h"
#include "CredentialDialog.h"
#include "DiagnosisGui.h"
#include "Env.h"
#include "generic/HelpAction.h"
#include "GuiProfile.h"
#include "NetworkManager.h"
#include "ReaderManager.h"
#include "RemoteClient.h"
#include "Service.h"
#include "SetupAssistantGui.h"
#include "UpdateWindow.h"
#include "workflow/WorkflowAuthenticateQtGui.h"
#include "workflow/WorkflowChangePinQtGui.h"
#include "workflow/WorkflowGui.h"
#include "workflow/WorkflowSelfInfoQtGui.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFile>
#include <QHBoxLayout>
#include <QLoggingCategory>
#include <QMenu>
#include <QMessageBox>

#ifndef QT_NO_NETWORKPROXY
	#include <QAuthenticator>
	#include <QNetworkProxy>
#endif


#if defined(Q_OS_MACOS)
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
	, mUpdateInfo(nullptr)
	, mCertificateInfo(nullptr)
	, mAggressiveToForeground(false)
{
	loadStyleSheet();

	mMainWidget = new AppQtMainWidget();
	mMainWidget->setWindowIcon(mIcon);

	mUpdateInfo = new QMessageBox(mMainWidget);
	mUpdateInfo->setWindowTitle(QApplication::applicationName() + QStringLiteral(" - ") + tr("Updates"));
	mUpdateInfo->setWindowIcon(QIcon(QStringLiteral(":/images/npa.svg")));
	mUpdateInfo->setWindowModality(Qt::WindowModal);
	mUpdateInfo->setStandardButtons(QMessageBox::Ok);
	mUpdateInfo->button(QMessageBox::Ok)->setFocus();

	mCertificateInfo = new QMessageBox(mMainWidget);
	mCertificateInfo->setWindowModality(Qt::ApplicationModal);
	mCertificateInfo->setWindowFlags(mCertificateInfo->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	mCertificateInfo->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
	mCertificateInfo->setIconPixmap(mMainWidget->windowIcon().pixmap(QSize(48, 48)));
	mCertificateInfo->setStandardButtons(QMessageBox::Ok);
	mCertificateInfo->button(QMessageBox::Ok)->setFocus();

	Service* service = Env::getSingleton<Service>();
	connect(service, &Service::fireAppUpdateFinished, this, &AppQtGui::onAppUpdateReady);
	connect(service, &Service::fireUpdateScheduled, this, &AppQtGui::onUpdateScheduled);
}


AppQtGui::~AppQtGui()
{
	if (mTrayIcon != nullptr)
	{
		QMenu* menu = mTrayIcon->contextMenu();
		if (menu != nullptr)
		{
			qDeleteAll(menu->actions());
			delete menu;
		}
	}

	delete mUpdateInfo;
	delete mCertificateInfo;
	delete mMainWidget;
}


void AppQtGui::init()
{
	connect(mMainWidget, &AppQtMainWidget::fireChangePinRequested, this, &AppQtGui::fireChangePinRequested);
	connect(mMainWidget, &AppQtMainWidget::fireSetupAssistantWizardRequest, this, &AppQtGui::onSetupAssistantWizardRequest);
	connect(mMainWidget, &AppQtMainWidget::fireDiagnosisRequested, this, &AppQtGui::onDiagnosisRequested);
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
		mTrayIcon->showMessage(QString(), tr("AusweisApp2 was started."), mIcon, 3000);
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

	const QSharedPointer<RemoteClient>& remoteClient = Env::getSingleton<ReaderManager>()->getRemoteClient();
	connect(remoteClient.data(), &RemoteClient::fireCertificateRemoved, this, &AppQtGui::onCertificateRemoved);
}


QSharedPointer<WorkflowAuthenticateQtGui> AppQtGui::createWorkflowAuthenticateUi(const QSharedPointer<AuthContext>& pContext)
{
	const QSharedPointer<WorkflowAuthenticateQtGui> gui(new WorkflowAuthenticateQtGui(pContext, mMainWidget));
	connect(gui.data(), &WorkflowGui::fireSwitchToReaderSettingsRequested, this, &AppQtGui::onSwitchToReaderSettingsRequested);

	return gui;
}


QSharedPointer<WorkflowChangePinQtGui> AppQtGui::createWorkflowChangePinUi(const QSharedPointer<ChangePinContext>& pContext)
{
	return QSharedPointer<WorkflowChangePinQtGui>(new WorkflowChangePinQtGui(pContext, mMainWidget));
}


QSharedPointer<WorkflowSelfInfoQtGui> AppQtGui::createWorkflowSelfInfoUi(const QSharedPointer<SelfAuthContext>& pContext)
{
	const QSharedPointer<WorkflowSelfInfoQtGui> gui(new WorkflowSelfInfoQtGui(pContext, mMainWidget));
	connect(gui.data(), &WorkflowGui::fireSwitchToReaderSettingsRequested, this, &AppQtGui::onSwitchToReaderSettingsRequested);

	return gui;
}


void AppQtGui::onSwitchToReaderSettingsRequested()
{
	mMainWidget->setHideWindowAfterWorkflow(false);

	Q_EMIT fireSwitchToReaderSettingsRequested();
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
	msgBox.setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText(pInformationMessage);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.button(QMessageBox::Ok)->setFocus();
	msgBox.exec();
}


void AppQtGui::onSetupAssistantWizardRequest()
{
	Env::getSingleton<Service>()->runUpdateIfNeeded();

	if (!mSetupAssistantGui)
	{
		mSetupAssistantGui = new SetupAssistantGui(mMainWidget);
		connect(mSetupAssistantGui, &SetupAssistantGui::fireChangePinButtonClicked, mMainWidget, &AppQtMainWidget::onChangePinButtonClicked);
	}

	bool stopRemoteScan = false;
	if (!mMainWidget->remoteScanRunning())
	{
		Env::getSingleton<ReaderManager>()->startScanAll(false);
		stopRemoteScan = true;
	}
	mSetupAssistantGui->activate();
	if (stopRemoteScan)
	{
		Env::getSingleton<ReaderManager>()->stopScanAll();
	}
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
	msgBox.button(QMessageBox::Yes)->setFocus();

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
	messageBox.setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
	messageBox.setWindowModality(Qt::WindowModal);
	messageBox.setWindowFlags(messageBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	messageBox.setText(tr("Did you change the initial transport PIN already?<br><br>Prior to the first use of the online identification function you have to replace the transport PIN by an individual 6-digit PIN. Online identification with transport PIN is not possible."));
	messageBox.setStandardButtons(QMessageBox::Yes);
	messageBox.button(QMessageBox::Yes)->setFocus();
	auto changePinButton = messageBox.addButton(tr("No, change transport PIN now"), QMessageBox::NoRole);
	messageBox.exec();

	if (messageBox.clickedButton() != changePinButton)
	{
		show(UiModule::IDENTIFY);
		return false;
	}

	return true;
}


void AppQtGui::switchToReaderSettings()
{
	mMainWidget->switchToGuiModule(GuiModule::DEVICE_SETTINGS);
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


void AppQtGui::loadStyleSheet()
{
	const auto& styleSheetName = QStringLiteral(":/stylesheets/desktop.qss");
	qCDebug(gui) << "loading style sheet" << styleSheetName;

	QFile file(styleSheetName);
	if (!file.open(QIODevice::ReadOnly))
	{
		qCWarning(gui) << "Failed to read style sheet";
		return;
	}

	qApp->setStyleSheet(QString::fromLatin1(file.readAll()));
}


void AppQtGui::onChangeHighContrast(bool* pHighContrastOn)
{
	if (*pHighContrastOn)
	{
		qApp->setStyleSheet(QString());
	}
	else
	{
		loadStyleSheet();
	}
}


void AppQtGui::createTrayIcon()
{
	QMenu* trayIconMenu = new QMenu(nullptr);

#if defined(Q_OS_MACOS)
	QAction* showApplicationAction = new QAction(tr("Open"), trayIconMenu);
	connect(showApplicationAction, &QAction::triggered, this, [this] {
				AppQtGui::show();
			});
#endif

	QAction* quitAction = new QAction(tr("Exit AusweisApp2"), trayIconMenu);
	connect(quitAction, &QAction::triggered, this, &AppQtGui::quitApplicationRequested);

#if defined(Q_OS_MACOS)
	trayIconMenu->addAction(showApplicationAction);
	trayIconMenu->addSeparator();
#endif
	trayIconMenu->addAction(quitAction);

	mTrayIcon = new QSystemTrayIcon(mIcon, mMainWidget);
	connect(mTrayIcon, &QSystemTrayIcon::activated, this, &AppQtGui::onActivated);
	connect(mTrayIcon, &QSystemTrayIcon::messageClicked, this, [this] {
				show(UiModule::CURRENT);
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
}


void AppQtGui::hideFromTaskbar()
{
#if defined(Q_OS_MACOS)
	ProcessSerialNumber psn = {
		0, kCurrentProcess
	};
	TransformProcessType(&psn, kProcessTransformToBackgroundApplication);
#endif
}


void AppQtGui::restoreToTaskbar()
{
#if defined(Q_OS_MACOS)
	ProcessSerialNumber psn = {
		0, kCurrentProcess
	};
	TransformProcessType(&psn, kProcessTransformToForegroundApplication);
	[NSApp activateIgnoringOtherApps: YES];
#endif
}


void AppQtGui::onActivated(QSystemTrayIcon::ActivationReason pReason)
{
#ifdef Q_OS_MACOS
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
		messageBox.setWindowTitle(QApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
		messageBox.setWindowModality(Qt::WindowModal);
		messageBox.setIcon(QMessageBox::Information);
		messageBox.setWindowFlags(messageBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
		messageBox.setText(tr("The user interface of the %1 is closed.").arg(QApplication::applicationName()));
		messageBox.setInformativeText(tr("The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.").arg(QApplication::applicationName()));
		messageBox.setCheckBox(new QCheckBox(tr("Do not show this dialog again.")));
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.button(QMessageBox::Ok)->setFocus();
		messageBox.exec();

		Q_EMIT fireCloseReminderFinished(messageBox.checkBox()->isChecked());
	}

	mMainWidget->onCloseWindowRequested();

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

		case UiModule::IDENTIFY:
			mMainWidget->switchToGuiModule(GuiModule::IDENTIFY);
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

	Env::getSingleton<Service>()->runUpdateIfNeeded();
}


void AppQtGui::onAppUpdateReady(bool pSuccess, const GlobalStatus& pError)
{
	if (pError.isError())
	{
		mUpdateInfo->setIcon(QMessageBox::Critical);
		mUpdateInfo->setText(pError.toErrorDescription());
		mUpdateInfo->exec();
	}
	else if (pSuccess)
	{
		const auto updateWindow = new UpdateWindow(mMainWidget);
		connect(updateWindow, &UpdateWindow::fireShowUpdateDialog, this,
				[this](QMessageBox::Icon pIcon, const QString& pMsg)
				{
					mUpdateInfo->setIcon(pIcon);
					mUpdateInfo->setText(pMsg);
					mUpdateInfo->exec();
				}
				);
	}
	else
	{
		mUpdateInfo->setIcon(QMessageBox::Information);
		mUpdateInfo->setText(tr("Your software is up to date."));
		mUpdateInfo->exec();
	}
}


void AppQtGui::onUpdateScheduled()
{
	if (!mMainWidget->isHidden())
	{
		Env::getSingleton<Service>()->runUpdateIfNeeded();
	}
}


void AppQtGui::onCertificateRemoved(QString pDeviceName)
{
	mCertificateInfo->setText(tr("The device \"%1\" was unpaired because it does not react to connection attempts. Retry the pairing process if you want to use this device to authenticate yourself.").arg(pDeviceName));
	mCertificateInfo->show();
}


void AppQtGui::shutdown()
{
	if (mTrayIcon != nullptr)
	{
		mTrayIcon->hide();
	}
}
