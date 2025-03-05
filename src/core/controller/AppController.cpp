/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AppController.h"

#include "AppSettings.h"
#include "LanguageLoader.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "Randomizer.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "SecureStorage.h"
#include "UiLoader.h"
#include "UiPlugin.h"
#include "VolatileSettings.h"
#include "controller/ChangePinController.h"

#include <QCoreApplication>
#include <QDir>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QTimer>

#if defined(Q_OS_WIN)
	#include <windows.h>
#endif


namespace governikus
{
bool AppController::cShowUi = false;
} // namespace governikus


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(init)
Q_DECLARE_LOGGING_CATEGORY(support)
Q_DECLARE_LOGGING_CATEGORY(system)


AppController::AppController()
	: mActiveWorkflow()
	, mWaitingRequest()
	, mShutdownRunning(false)
	, mUiDomination(nullptr)
	, mRestartApplication(false)
	, mExitCode(EXIT_SUCCESS)
{
	setObjectName(QStringLiteral("AppController"));

#if defined(Q_OS_WIN)
	QCoreApplication::instance()->installNativeEventFilter(this);
#endif

	Randomizer::getInstance(); // just init entropy pool
	Env::getSingleton<VolatileSettings>(); // just init in MainThread because of QObject
	ResourceLoader::getInstance().init();

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireLanguageChanged, this, &AppController::onLanguageChanged);
	onLanguageChanged();

	NetworkManager::setApplicationProxyFactory();
	connect(Env::getSingleton<NetworkManager>(), &NetworkManager::fireProxyAuthenticationRequired, this, &AppController::fireProxyAuthenticationRequired);

	connect(this, &AppController::fireShutdown, Env::getSingleton<NetworkManager>(), &NetworkManager::onShutdown, Qt::QueuedConnection);
}


bool AppController::canStartNewWorkflow() const
{
	return mActiveWorkflow.isNull();
}


bool AppController::eventFilter(QObject* pObj, QEvent* pEvent)
{
#ifdef Q_OS_MACOS
	if (pEvent)
	{
		// This event gets sent on reopen events in macOS (opening the app again while
		// it is in the background). Currently the event handling is only needed on macOS.
		if (pEvent->type() == QEvent::ApplicationActivate)
		{
			qCDebug(system) << "Got an ApplicationActivate event, showing current UI Workflow.";
			Q_EMIT fireShowUi(UiModule::CURRENT);
		}
		else if (pEvent->type() == QEvent::Close && pObj == QCoreApplication::instance())
		{
			qCDebug(system) << "Got CloseEvent from system, hiding UI.";
			Q_EMIT fireHideUi();
		}
	}
#endif

	if (pEvent->type() == QEvent::Quit)
	{
		doShutdown();
		return true;
	}

	if (pEvent->type() == QEvent::ApplicationActivated)
	{
		Q_EMIT fireApplicationActivated();
	}

	return QObject::eventFilter(pObj, pEvent);
}


void AppController::start()
{
	auto shutdownGuard = qScopeGuard([this] {
				qCCritical(init) << "Cannot start application controller, exit application";

				// Let's enter event loop before we shutdown.
				// Otherwise we call QCoreApplication::exit before ::exec and get stuck.
				QMetaObject::invokeMethod(this, [this]{
					doShutdown(EXIT_FAILURE);
				}, Qt::QueuedConnection);
			});

	if (!Env::getSingleton<SecureStorage>()->isValid())
	{
		qCCritical(init) << "SecureStorage not valid";
		return;
	}

#if !defined(Q_OS_ANDROID)
	if (Env::getSingleton<AppSettings>()->getGeneralSettings().isNewAppVersion())
	{
		// Cache cleanup on android is handled by UpdateReceiver.java
		clearCacheFolders();
	}
#endif

	auto* uiLoader = Env::getSingleton<UiLoader>();
	connect(uiLoader, &UiLoader::fireLoadedPlugin, this, &AppController::onUiPlugin);
	if (!uiLoader->load() || !uiLoader->hasActiveUI())
	{
		qCCritical(init) << "Cannot start without active UI";
		return;
	}

	if (!uiLoader->initialize())
	{
		qCCritical(init) << "Cannot initialize UI";
		return;
	}

	if (uiLoader->requiresReaderManager())
	{
		// Start the ReaderManager *after* initializing the UIs. Otherwise the TrayIcon
		// will be created even when we're just showing an error message and shutdown after that.
		auto* readerManager = Env::getSingleton<ReaderManager>();
		connect(this, &AppController::fireShutdown, readerManager, &ReaderManager::shutdown, Qt::QueuedConnection);
		connect(readerManager, &ReaderManager::fireInitialized, this, &AppController::fireStarted, Qt::QueuedConnection);
		readerManager->init();
	}
	else
	{
		connect(this, &AppController::fireInitialized, this, &AppController::fireStarted, Qt::QueuedConnection);
	}

	connect(this, &AppController::fireStarted, this, [this] {
				if (cShowUi)
				{
					Q_EMIT fireShowUi(UiModule::CURRENT);
				}
			}, Qt::QueuedConnection);

	QCoreApplication::instance()->installEventFilter(this);

	Q_EMIT fireInitialized();
	shutdownGuard.dismiss();
}


bool AppController::shouldApplicationRestart() const
{
	return mRestartApplication;
}


void AppController::onWorkflowFinished()
{
	Q_ASSERT(mActiveWorkflow);
	Q_ASSERT(mActiveWorkflow->getController());

	auto controller = mActiveWorkflow->getController();

	qDebug() << controller->metaObject()->className() << "done";
	disconnect(controller.data(), &WorkflowController::fireComplete, this, &AppController::onWorkflowFinished);

	Q_EMIT fireWorkflowFinished(mActiveWorkflow);

	qCInfo(support) << "Finish workflow" << mActiveWorkflow->getAction();
	mActiveWorkflow.reset();

	if (!mWaitingRequest.isNull())
	{
		qDebug() << "Running waiting action now.";
		startNewWorkflow(mWaitingRequest);
		mWaitingRequest.reset();
	}

	if (mShutdownRunning)
	{
		completeShutdown();
	}
}


void AppController::onWorkflowRequested(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_ASSERT(pRequest && !pRequest->isInitialized());
	qDebug() << "New workflow requested:" << pRequest->getAction();

	if (canStartNewWorkflow())
	{
		startNewWorkflow(pRequest);
		return;
	}

	switch (pRequest->handleBusyWorkflow(mActiveWorkflow, mWaitingRequest))
	{
		case WorkflowControl::UNHANDLED:
			qWarning() << "Cannot start or enqueue workflow:" << pRequest->getAction();
			Q_EMIT fireWorkflowUnhandled(pRequest);
			break;

		case WorkflowControl::SKIP:
			qWarning() << "Skip workflow:" << pRequest->getAction() << "| Current workflow:" << mActiveWorkflow->getAction();
			if (mWaitingRequest)
			{
				qDebug() << "Waiting workflow:" << mWaitingRequest->getAction();
			}
			Q_EMIT fireWorkflowUnhandled(pRequest);
			break;

		case WorkflowControl::ENQUEUE:
			Q_ASSERT(mWaitingRequest.isNull());
			mWaitingRequest = pRequest;
			qDebug() << "Enqueue workflow:" << mWaitingRequest->getAction();
			mActiveWorkflow->getContext()->setNextWorkflowPending(true);
			break;
	}
}


void AppController::onCloseReminderFinished(bool pDontRemindAgain) const
{
	if (pDontRemindAgain)
	{
		Env::getSingleton<AppSettings>()->getGeneralSettings().setRemindUserToClose(false);
	}
}


void AppController::onLanguageChanged()
{
	LanguageLoader& languageLoader = LanguageLoader::getInstance();
	const QLocale& newLocale = QLocale(Env::getSingleton<AppSettings>()->getGeneralSettings().getLanguage());
	const QLocale& usedLocale = languageLoader.getUsedLocale();

	if (newLocale == usedLocale)
	{
		return;
	}

	if (languageLoader.isLoaded())
	{
		languageLoader.unload();
	}

	if (newLocale == QLocale::C)
	{
		languageLoader.load();
	}
	else
	{
		languageLoader.load(newLocale);
	}

	Q_EMIT fireTranslationChanged();
}


void AppController::doShutdown(int pExitCode)
{
	if (mShutdownRunning)
	{
		qCDebug(init) << "Application already shutting down";
		return;
	}

	mExitCode = pExitCode;
	mShutdownRunning = true;

	if (mActiveWorkflow)
	{
		// Make sure that any request for a new workflow is removed from the queue.
		mWaitingRequest.reset();

		// Make sure running workflow is not blocked by any open dialogs
		// (hiding the GUI also closes open dialogs).
		Q_EMIT fireHideUi();

		// Make sure the workflow runs to the end without user interaction.
		const QSharedPointer<WorkflowContext> context = mActiveWorkflow->getContext();
		if (context)
		{
			context->killWorkflow();
		}
	}
	else
	{
		// Make sure the GUI is not blocked by any open dialogs
		// (hiding the GUI also closes open dialogs).
		Q_EMIT fireHideUi();

		completeShutdown();
	}
}


void AppController::completeShutdown()
{
	qCDebug(init) << "Emit fire shutdown";
	Q_EMIT fireShutdown();

	const auto& uiShutdown = [this] {
				const auto& exitApp = [this] {
							ResourceLoader::getInstance().shutdown();
							qCDebug(init) << "Quit event loop of QCoreApplication";
							QCoreApplication::exit(mExitCode);
						};

				auto* uiLoader = Env::getSingleton<UiLoader>();
				if (uiLoader->isLoaded())
				{
					connect(uiLoader, &UiLoader::fireRemovedAllPlugins, this, exitApp, Qt::QueuedConnection);
					uiLoader->shutdown();
				}
				else
				{
					exitApp();
				}
			};

	if (Env::getSingleton<NetworkManager>()->getOpenConnectionCount() > 0)
	{
		waitForNetworkConnections(uiShutdown);
	}
	else
	{
		uiShutdown();
	}
}


void AppController::waitForNetworkConnections(const std::function<void()>& pExitFunc)
{
	auto* timer = new QTimer(this);
	static const int TIMER_INTERVAL = 50;
	timer->setInterval(TIMER_INTERVAL);
	connect(timer, &QTimer::timeout, this, [timer, pExitFunc](){
				const int openConnectionCount = Env::getSingleton<NetworkManager>()->getOpenConnectionCount();
				if (openConnectionCount > 0)
				{
					static int timesInvoked = 0;
					const int THREE_SECONDS = 3000;
					if (++timesInvoked < THREE_SECONDS / TIMER_INTERVAL)
					{
						return;
					}

					qCWarning(init) << "Closing with" << openConnectionCount << "pending network connections.";
					Q_ASSERT(false);
				}

				timer->deleteLater();
				pExitFunc();
			});

	timer->start();
}


void AppController::onUiDominationRequested(const UiPlugin* pUi, const QString& pInformation)
{
	bool accepted = false;
	if (mUiDomination == nullptr && canStartNewWorkflow())
	{
		mUiDomination = pUi;
		accepted = true;
	}

	qDebug() << pUi->metaObject()->className() << "requested ui domination:" << pInformation << '|' << accepted;
	Q_EMIT fireUiDomination(pUi, pInformation, accepted);
}


void AppController::onUiDominationRelease()
{
	if (mUiDomination)
	{
		qDebug() << mUiDomination->metaObject()->className() << "released ui domination";
		mUiDomination = nullptr;
		Q_EMIT fireUiDominationReleased();
	}
}


void AppController::onRestartApplicationRequested()
{
	mRestartApplication = true;
	doShutdown();
}


void AppController::onUiPlugin(const UiPlugin* pPlugin) const
{
	qCDebug(init) << "Register UI:" << pPlugin->metaObject()->className();
	connect(this, &AppController::fireShutdown, pPlugin, &UiPlugin::doShutdown, Qt::QueuedConnection);
	connect(this, &AppController::fireWorkflowStarted, pPlugin, &UiPlugin::onWorkflowStarted);
	connect(this, &AppController::fireWorkflowFinished, pPlugin, &UiPlugin::onWorkflowFinished);
	connect(this, &AppController::fireWorkflowUnhandled, pPlugin, &UiPlugin::onWorkflowUnhandled);
	connect(this, &AppController::fireInitialized, pPlugin, &UiPlugin::onApplicationInitialized);
	connect(this, &AppController::fireStarted, pPlugin, &UiPlugin::onApplicationStarted);
	connect(this, &AppController::fireShowUi, pPlugin, &UiPlugin::onShowUi);
	connect(this, &AppController::fireHideUi, pPlugin, &UiPlugin::onHideUi);
	connect(this, &AppController::fireTranslationChanged, pPlugin, &UiPlugin::onTranslationChanged);
	connect(this, &AppController::fireShowUserInformation, pPlugin, &UiPlugin::onShowUserInformation);
	connect(this, &AppController::fireApplicationActivated, pPlugin, &UiPlugin::fireApplicationActivated);
	connect(this, &AppController::fireSystemSettingsChanged, pPlugin, &UiPlugin::fireSystemSettingsChanged);
	connect(this, &AppController::fireUiDomination, pPlugin, &UiPlugin::onUiDomination);
	connect(this, &AppController::fireUiDominationReleased, pPlugin, &UiPlugin::onUiDominationReleased);
	connect(this, &AppController::fireProxyAuthenticationRequired, pPlugin, &UiPlugin::onProxyAuthenticationRequired);

	connect(pPlugin, &UiPlugin::fireWorkflowRequested, this, &AppController::onWorkflowRequested, Qt::QueuedConnection);
	connect(pPlugin, &UiPlugin::fireShowUiRequested, this, &AppController::fireShowUi, Qt::QueuedConnection);
	connect(pPlugin, &UiPlugin::fireShowUserInformationRequested, this, &AppController::fireShowUserInformation);
	connect(pPlugin, &UiPlugin::fireRestartApplicationRequested, this, &AppController::onRestartApplicationRequested, Qt::QueuedConnection);
	connect(pPlugin, &UiPlugin::fireQuitApplicationRequest, this, &AppController::doShutdown);
	connect(pPlugin, &UiPlugin::fireCloseReminderFinished, this, &AppController::onCloseReminderFinished);
	connect(pPlugin, &UiPlugin::fireUiDominationRequest, this, &AppController::onUiDominationRequested);
	connect(pPlugin, &UiPlugin::fireUiDominationRelease, this, &AppController::onUiDominationRelease);
}


bool AppController::startNewWorkflow(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_ASSERT(pRequest && !pRequest->isInitialized());

	if (!canStartNewWorkflow())
	{
		qWarning() << "Cannot start new workflow:" << pRequest->getAction() << "| Current workflow:" << mActiveWorkflow->getAction();
		return false;
	}

	mActiveWorkflow = pRequest;
	mActiveWorkflow->initialize();
	qCInfo(support) << "Started new workflow" << mActiveWorkflow->getAction();
	auto controller = mActiveWorkflow->getController();
	connect(controller.data(), &WorkflowController::fireComplete, this, &AppController::onWorkflowFinished, Qt::QueuedConnection);
	Env::getSingleton<LogHandler>()->resetBacklog();
	qDebug() << "Start" << controller->metaObject()->className();

	//first: run new active controller so that it can be canceled during UI activation if required
	controller->run();

	//second: activate ui
	Q_EMIT fireWorkflowStarted(mActiveWorkflow);

	if (!mActiveWorkflow->getContext()->wasClaimed())
	{
		qCritical() << "Workflow was not claimed by any UI... aborting";
		mActiveWorkflow->getContext()->killWorkflow(GlobalStatus::Code::Workflow_InternalError_BeforeTcToken);
		return false;
	}

	return true;
}


bool AppController::nativeEventFilter(const QByteArray& pEventType, void* pMessage, qintptr* pResult)
{
	Q_UNUSED(pResult)
#if !defined(Q_OS_WIN)
	Q_UNUSED(pEventType)
	Q_UNUSED(pMessage)
#else
	if (pEventType == "windows_generic_MSG")
	{
		MSG* msg = reinterpret_cast<MSG*>(pMessage);
		if (msg->message == WM_QUERYENDSESSION)
		{
			qCDebug(system) << "WM_QUERYENDSESSION received";
			*pResult = TRUE;
			return true;
		}
		if (msg->message == WM_ENDSESSION)
		{
			qCDebug(system) << "WM_ENDSESSION received";
			*pResult = 0;
			doShutdown();
			return true;
		}
		if (msg->message == WM_SETTINGCHANGE)
		{
			qCDebug(system) << "WM_SETTINGCHANGE received";
			Q_EMIT fireSystemSettingsChanged();
		}
	}
#endif

	return false;
}


void AppController::clearCacheFolders()
{
	qDebug() << "Try to find and clear cache folder";

	const QStringList cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
	for (QString cachePath : cachePaths)
	{
		cachePath.replace(QStringLiteral("AusweisApp"), QStringLiteral("AusweisApp2"));
		auto cacheDir = QDir(cachePath);

		if (!cacheDir.exists())
		{
			continue;
		}

		cacheDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
		auto entries = cacheDir.entryList();
		for (const QString& filePath : std::as_const(entries))
		{
			cacheDir.remove(filePath);
		}

		cacheDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
		entries = cacheDir.entryList();
		for (const QString& subDirPath : std::as_const(entries))
		{
			QDir subDir(cacheDir.absoluteFilePath(subDirPath));
			subDir.removeRecursively();
		}

		qDebug() << "Cleared cache folder:" << cachePath;
	}
}
