/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AppQtGui.h"

#include "AppSettings.h"
#include "CredentialDialog.h"
#include "HelpAction.h"
#include "NetworkManager.h"
#include "PlatformTools.h"
#include "ReaderManager.h"
#include "RemoteClient.h"
#include "Service.h"
#include "workflow/WorkflowGui.h"

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


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


AppQtGui::AppQtGui()
	: QObject()
	, mMainWidget(new AppQtMainWidget())
	, mTrayIcon()
	, mActiveWorkflowUi()
	, mSetupAssistantGui(nullptr)
	, mDiagnosisGui(nullptr)
	, mUpdateInfo(new QMessageBox(mMainWidget))
	, mCertificateInfo(new QMessageBox(mMainWidget))
	, mLockedInfo(new QMessageBox(mMainWidget))
	, mSwitchUiInquiry(new QMessageBox(mMainWidget))
	, mUpdateWindow(new UpdateWindow(mMainWidget))
	, mAggressiveToForeground(false)
{
	loadStyleSheet();

	mMainWidget->setWindowIcon(mTrayIcon.getIcon());
	connect(mMainWidget, &AppQtMainWidget::fireCloseActiveDialogs, this, &AppQtGui::onCloseActiveDialogs);
	connect(mMainWidget, &AppQtMainWidget::fireSwitchUiRequested, this, &AppQtGui::onSwitchUiRequested);

	connect(&mTrayIcon, &TrayIcon::fireShow, this, [this] {
				AppQtGui::show();
			});
	connect(&mTrayIcon, &TrayIcon::fireQuit, this, &AppQtGui::quitApplicationRequested);

	mUpdateInfo->setWindowTitle(QApplication::applicationName() + QStringLiteral(" - ") + tr("Updates"));
	mUpdateInfo->setWindowIcon(mTrayIcon.getIcon());
	mUpdateInfo->setWindowModality(Qt::WindowModal);
	mUpdateInfo->setStandardButtons(QMessageBox::Ok);
	mUpdateInfo->button(QMessageBox::Ok)->setFocus();
	connect(mUpdateWindow, &UpdateWindow::fireShowUpdateDialog, this,
			[this](QMessageBox::Icon pIcon, const QString& pMsg)
			{
				mUpdateInfo->setIcon(pIcon);
				mUpdateInfo->setText(pMsg);
				mUpdateInfo->exec();
			});

	mCertificateInfo->setWindowModality(Qt::ApplicationModal);
	mCertificateInfo->setWindowFlags(mCertificateInfo->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	mCertificateInfo->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
	mCertificateInfo->setIconPixmap(mMainWidget->windowIcon().pixmap(QSize(48, 48)));
	mCertificateInfo->setStandardButtons(QMessageBox::Ok);
	mCertificateInfo->button(QMessageBox::Ok)->setFocus();

	mLockedInfo->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("SDK"));
	mLockedInfo->setWindowFlags(mLockedInfo->windowFlags() & ~Qt::WindowCloseButtonHint& ~Qt::WindowContextHelpButtonHint& ~Qt::WindowMinMaxButtonsHint);
	mLockedInfo->setWindowIcon(mTrayIcon.getIcon());
	mLockedInfo->setWindowModality(Qt::WindowModal);
	mLockedInfo->setIcon(QMessageBox::Information);
	mLockedInfo->setText(tr("Another application uses AusweisApp2."));
	mLockedInfo->setStandardButtons(QMessageBox::NoButton);

	mSwitchUiInquiry->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Switch UI"));
	mSwitchUiInquiry->setIcon(QMessageBox::Information);
	mSwitchUiInquiry->setText(tr("Do you want to switch to the new beta UI? You can switch back to the old UI in \"Settings\"."));
	mSwitchUiInquiry->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	mSwitchUiInquiry->button(QMessageBox::Yes)->setFocus();

	auto* service = Env::getSingleton<Service>();
	connect(service, &Service::fireAppUpdateFinished, this, &AppQtGui::onAppUpdateReady);
	connect(service, &Service::fireUpdateScheduled, this, &AppQtGui::onUpdateScheduled);
}


AppQtGui::~AppQtGui()
{
	delete mUpdateInfo;
	delete mCertificateInfo;
	delete mLockedInfo;
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
	mTrayIcon.create();

	if (!QSystemTrayIcon::isSystemTrayAvailable()
			|| Env::getSingleton<AppSettings>()->getGeneralSettings().isShowSetupAssistant()
			|| Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
	{
		QMetaObject::invokeMethod(this, [this] {
					show();
				}, Qt::QueuedConnection);
	}

	if (Env::getSingleton<AppSettings>()->getGeneralSettings().isShowSetupAssistant())
	{
		mMainWidget->setSelectedTab(nullptr);
		QMetaObject::invokeMethod(this, &AppQtGui::onSetupAssistantWizardRequest, Qt::QueuedConnection);

		// just show the setup assistant once
		Env::getSingleton<AppSettings>()->getGeneralSettings().setShowSetupAssistant(false);
		Env::getSingleton<AppSettings>()->getGeneralSettings().save();
	}

	if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
	{
		QMetaObject::invokeMethod(this, &AppQtGui::onDeveloperModeQuestion, Qt::QueuedConnection);
	}

	const auto& remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireCertificateRemoved, this, &AppQtGui::onCertificateRemoved);
}


QSharedPointer<WorkflowAuthenticateQtGui> AppQtGui::createWorkflowAuthenticateUi(const QSharedPointer<AuthContext>& pContext)
{
	return QSharedPointer<WorkflowAuthenticateQtGui>::create(pContext, mMainWidget);
}


QSharedPointer<WorkflowChangePinQtGui> AppQtGui::createWorkflowChangePinUi(const QSharedPointer<ChangePinContext>& pContext)
{
	return QSharedPointer<WorkflowChangePinQtGui>::create(pContext, mMainWidget);
}


QSharedPointer<WorkflowSelfInfoQtGui> AppQtGui::createWorkflowSelfInfoUi(const QSharedPointer<SelfAuthContext>& pContext)
{
	return QSharedPointer<WorkflowSelfInfoQtGui>::create(pContext, mMainWidget);
}


void AppQtGui::activateWorkflowUi(QSharedPointer<WorkflowGui> pWorkflowUi, bool pAllowHideAfterWorkflow)
{
	if (pWorkflowUi)
	{
		mActiveWorkflowUi = pWorkflowUi;
		mActiveWorkflowUi->activate();
		connect(this, &AppQtGui::fireCloseActiveDialogs, mActiveWorkflowUi.data(), &WorkflowGui::onCloseActiveDialogs);
	}

	mMainWidget->activateMenuBarItems(false);
	closeDialogs();

#ifdef Q_OS_WIN
	mAggressiveToForeground = mActiveWorkflowUi.objectCast<WorkflowAuthenticateQtGui>();
#endif
	bool hideAfterWorkflow = pAllowHideAfterWorkflow
			&& mActiveWorkflowUi.objectCast<WorkflowAuthenticateQtGui>()
			&& Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoCloseWindowAfterAuthentication();
	mMainWidget->setHideWindowAfterWorkflow(hideAfterWorkflow);
	show();
}


void AppQtGui::deactivateCurrentWorkflowUi()
{
	if (mMainWidget->isHideWindowAfterWorkflow())
	{
		PlatformTools::hideFromTaskbar();
		mMainWidget->hideWithoutConfirmation();
	}
	mMainWidget->activateMenuBarItems(true);

	if (mActiveWorkflowUi)
	{
		disconnect(this, &AppQtGui::fireCloseActiveDialogs, mActiveWorkflowUi.data(), &WorkflowGui::onCloseActiveDialogs);
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

	if (mSetupAssistantGui == nullptr)
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
		Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(false);
		Env::getSingleton<AppSettings>()->getGeneralSettings().save();
	}
}


void AppQtGui::onDiagnosisRequested()
{
	if (mDiagnosisGui == nullptr)
	{
		mDiagnosisGui = new DiagnosisGui(mMainWidget);
	}
	mDiagnosisGui->activate();
}


bool AppQtGui::askChangeTransportPinNow()
{
	GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
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
	messageBox.setText(tr("Did you change the transport PIN already?<br><br>Prior to the first use of the online identification function you have to replace the transport PIN by an individual 6-digit PIN. Online identification with transport PIN is not possible."));
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


bool AppQtGui::eventFilter(QObject* /*pObject*/, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		auto* keyEvent = static_cast<QKeyEvent*>(pEvent);
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
	if (mSwitchUiInquiry)
	{
		mSwitchUiInquiry->reject();
	}
}


void AppQtGui::onCloseWindowRequested(bool* pDoClose)
{
	if (mActiveWorkflowUi == nullptr && mMainWidget->isRemindUserToClose())
	{
		QMessageBox messageBox(mMainWidget);
		messageBox.installEventFilter(this);
		messageBox.setWindowTitle(QApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
		messageBox.setWindowModality(Qt::ApplicationModal);
		messageBox.setIcon(QMessageBox::Information);
		messageBox.setWindowFlags(messageBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
		messageBox.setText(tr("The user interface of the %1 is closed.").arg(QApplication::applicationName()));
		messageBox.setInformativeText(tr("The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.").arg(QApplication::applicationName()));
		messageBox.setCheckBox(new QCheckBox(tr("Do not show this dialog again.")));
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.button(QMessageBox::Ok)->setFocus();
		connect(this, &AppQtGui::fireCloseActiveDialogs, &messageBox, &QMessageBox::reject);
		messageBox.exec();

		Q_EMIT fireCloseReminderFinished(messageBox.checkBox()->isChecked());
	}
	mMainWidget->onCloseWindowRequested();

	if (mActiveWorkflowUi == nullptr)
	{
		*pDoClose = true;
	}
	else
	{
		connect(this, &AppQtGui::fireCloseActiveDialogs, mActiveWorkflowUi.data(), &WorkflowGui::fireCloseActiveDialogs);
		if ((*pDoClose = mActiveWorkflowUi->verifyAbortWorkflow()))
		{
			Q_EMIT mActiveWorkflowUi->fireUserCancelled();
		}
	}

	if (*pDoClose)
	{
		PlatformTools::hideFromTaskbar();
	}
}


void AppQtGui::onSwitchUiRequested()
{
	if (mSwitchUiInquiry->exec() != QMessageBox::Yes)
	{
		return;
	}

	auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	generalSettings.setSelectedUi(QStringLiteral("qml"));
	generalSettings.save();

	Q_EMIT fireRestartApplicationRequested();
}


void AppQtGui::onCloseActiveDialogs()
{
	if (mSetupAssistantGui != nullptr)
	{
		mSetupAssistantGui->deactivate();
	}

	Q_EMIT fireCloseActiveDialogs();
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

	PlatformTools::restoreToTaskbar();

	Env::getSingleton<Service>()->runUpdateIfNeeded();
}


void AppQtGui::hideWithoutConfirmation()
{
	mMainWidget->hideWithoutConfirmation();
}


void AppQtGui::setEnabled(bool pEnable, const QString& pLockedInfoText)
{
	mMainWidget->setEnabled(pEnable);
	if (pEnable)
	{
		mLockedInfo->hide();
	}
	else
	{
		const auto& txt = pLockedInfoText.isEmpty() ? QString() : tr("Application: %1").arg(pLockedInfoText);
		mLockedInfo->setInformativeText(txt);
		mLockedInfo->open();
		mLockedInfo->setEnabled(true);
	}
}


bool AppQtGui::isEnabled() const
{
	return mMainWidget->isEnabled();
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
		mUpdateWindow->fillData();
		mUpdateWindow->show();
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


void AppQtGui::onCertificateRemoved(const QString& pDeviceName)
{
	mCertificateInfo->setText(tr("The device \"%1\" was unpaired because it does not react to connection attempts. Retry the pairing process if you want to use this device to authenticate yourself.").arg(pDeviceName));
	mCertificateInfo->show();
}


void AppQtGui::shutdown()
{
	Q_EMIT fireCloseActiveDialogs();
	mTrayIcon.hide();
}
