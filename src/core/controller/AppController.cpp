/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
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
#include "UILoader.h"
#include "UIPlugIn.h"
#include "VolatileSettings.h"
#include "controller/AuthController.h"
#include "controller/ChangePinController.h"

#include <QCoreApplication>
#include <QDir>
#include <QLoggingCategory>
#include <QSettings>
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


Q_DECLARE_LOGGING_CATEGORY(support)
Q_DECLARE_LOGGING_CATEGORY(system)


AppController::AppController()
	: mActiveWorkflow()
	, mWaitingRequest()
	, mActivationController()
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

	if (pEvent->type() == QEvent::ApplicationActivated)
	{
		Q_EMIT fireApplicationActivated();
	}

	return QObject::eventFilter(pObj, pEvent);
}


bool AppController::start()
{
	if (!Env::getSingleton<SecureStorage>()->isLoaded())
	{
		qCritical() << "SecureStorage not loaded";
		return false;
	}

#if !defined(Q_OS_ANDROID)
	if (Env::getSingleton<AppSettings>()->getGeneralSettings().isNewAppVersion())
	{
		// Cache cleanup on android is handled by UpdateReceiver.java
		clearCacheFolders();
	}
#endif

	auto* uiLoader = Env::getSingleton<UILoader>();
	connect(uiLoader, &UILoader::fireLoadedPlugin, this, &AppController::onUiPlugin);
	if (!uiLoader->load())
	{
		qCritical() << "Cannot start without UI";
		return false;
	}

	for (const auto& handler : mActivationController.getHandler())
	{
		connect(this, &AppController::fireApplicationActivated, handler, &ActivationHandler::onApplicationActivated);
		connect(handler, &ActivationHandler::fireShowUserInformation, this, &AppController::fireShowUserInformation);
		connect(handler, &ActivationHandler::fireShowUiRequest, this, &AppController::fireShowUi);
		connect(handler, &ActivationHandler::fireAuthenticationRequest, this, &AppController::onAuthenticationContextRequest, Qt::QueuedConnection);

		if (!handler->start())
		{
			qCritical() << "Cannot start activation handler:" << handler->metaObject()->className();
			doShutdown();
			return false;
		}
		qDebug() << "Successfully started activation handler:" << handler->metaObject()->className();
	}

	// Start the ReaderManager *after* initializing the ActivationHandlers. Otherwise the TrayIcon
	// will be created even when we're just showing an error message and shutdown after that.
	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(this, &AppController::fireShutdown, readerManager, &ReaderManager::shutdown, Qt::QueuedConnection);
	connect(readerManager, &ReaderManager::fireInitialized, this, &AppController::fireStarted, Qt::QueuedConnection);
	connect(this, &AppController::fireStarted, this, [this] {
			if (cShowUi)
			{
				Q_EMIT fireShowUi(UiModule::CURRENT);
			}
		}, Qt::QueuedConnection);

	readerManager->init();
	QCoreApplication::instance()->installEventFilter(this);

	Q_EMIT fireInitialized();

	return true;
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

	if (mActiveWorkflow->getContext()->getLastPaceResult() == CardReturnCode::NO_ACTIVE_PIN_SET)
	{
		switch (mActiveWorkflow->getAction())
		{
			case Action::AUTH:
			case Action::SELF:
			case Action::PERSONALIZATION:
				onWorkflowRequested(ChangePinController::createWorkflowRequest(true));
				break;

			case Action::PIN:
			case Action::REMOTE_SERVICE:
				break;
		}
	}

	Q_EMIT fireWorkflowFinished(mActiveWorkflow->getContext());

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
			break;

		case WorkflowControl::SKIP:
			qWarning() << "Skip workflow:" << pRequest->getAction() << "| Current workflow:" << mActiveWorkflow->getAction();
			if (mWaitingRequest)
			{
				qDebug() << "Waiting workflow:" << mWaitingRequest->getAction();
			}
			break;

		case WorkflowControl::ENQUEUE:
			Q_ASSERT(mWaitingRequest.isNull());
			mWaitingRequest = pRequest;
			qDebug() << "Enqueue workflow:" << mWaitingRequest->getAction();
			mActiveWorkflow->getContext()->setNextWorkflowPending(true);
			break;
	}
}


void AppController::onCloseReminderFinished(bool pDontRemindAgain)
{
	if (pDontRemindAgain)
	{
		Env::getSingleton<AppSettings>()->getGeneralSettings().setRemindUserToClose(false);
	}
}


void AppController::onAuthenticationContextRequest(const QSharedPointer<ActivationContext>& pActivationContext)
{
	onWorkflowRequested(AuthController::createWorkflowRequest(pActivationContext));
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
		qDebug() << "Application already shutting down";
		return;
	}

	mExitCode = pExitCode;
	mShutdownRunning = true;
	mActivationController.shutdown();

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
	qDebug() << "Emit fire shutdown";
	Q_EMIT fireShutdown();

	ResourceLoader::getInstance().shutdown();

	auto* timer = new QTimer(this);
	static const int TIMER_INTERVAL = 50;
	timer->setInterval(TIMER_INTERVAL);
	connect(timer, &QTimer::timeout, this, [this, timer](){
			const int openConnectionCount = Env::getSingleton<NetworkManager>()->getOpenConnectionCount();
			if (openConnectionCount > 0)
			{
				static int timesInvoked = 0;
				const int THREE_SECONDS = 3000;
				if (++timesInvoked < THREE_SECONDS / TIMER_INTERVAL)
				{
					return;
				}

				qWarning() << "Closing with" << openConnectionCount << "pending network connections.";
				Q_ASSERT(false);
			}

			timer->deleteLater();

			auto* uiLoader = Env::getSingleton<UILoader>();
			connect(uiLoader, &UILoader::fireShutdownComplete, this, &AppController::onUILoaderShutdownComplete, Qt::QueuedConnection);
			QMetaObject::invokeMethod(uiLoader, &UILoader::shutdown, Qt::QueuedConnection);
		});

	timer->start();
}


void AppController::onUILoaderShutdownComplete()
{
	qDebug() << "Quit event loop of QCoreApplication";
	QCoreApplication::exit(mExitCode);
}


void AppController::onUiDominationRequested(const UIPlugIn* pUi, const QString& pInformation)
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
		mUiDomination = nullptr;
		Q_EMIT fireUiDominationReleased();
	}
}


void AppController::onRestartApplicationRequested()
{
	mRestartApplication = true;
	doShutdown();
}


void AppController::onUiPlugin(const UIPlugIn* pPlugin)
{
	qDebug() << "Register UI:" << pPlugin->metaObject()->className();
	connect(this, &AppController::fireShutdown, pPlugin, &UIPlugIn::doShutdown, Qt::QueuedConnection);
	connect(this, &AppController::fireWorkflowStarted, pPlugin, &UIPlugIn::onWorkflowStarted);
	connect(this, &AppController::fireWorkflowFinished, pPlugin, &UIPlugIn::onWorkflowFinished);
	connect(this, &AppController::fireInitialized, pPlugin, &UIPlugIn::onApplicationInitialized);
	connect(this, &AppController::fireStarted, pPlugin, &UIPlugIn::onApplicationStarted);
	connect(this, &AppController::fireShowUi, pPlugin, &UIPlugIn::onShowUi);
	connect(this, &AppController::fireHideUi, pPlugin, &UIPlugIn::onHideUi);
	connect(this, &AppController::fireTranslationChanged, pPlugin, &UIPlugIn::onTranslationChanged);
	connect(this, &AppController::fireShowUserInformation, pPlugin, &UIPlugIn::fireShowUserInformation);
	connect(this, &AppController::fireApplicationActivated, pPlugin, &UIPlugIn::fireApplicationActivated);
	connect(this, &AppController::fireUiDomination, pPlugin, &UIPlugIn::onUiDomination);
	connect(this, &AppController::fireUiDominationReleased, pPlugin, &UIPlugIn::onUiDominationReleased);
	connect(this, &AppController::fireProxyAuthenticationRequired, pPlugin, &UIPlugIn::onProxyAuthenticationRequired);

	connect(pPlugin, &UIPlugIn::fireWorkflowRequested, this, &AppController::onWorkflowRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireRestartApplicationRequested, this, &AppController::onRestartApplicationRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireQuitApplicationRequest, this, &AppController::doShutdown);
	connect(pPlugin, &UIPlugIn::fireCloseReminderFinished, this, &AppController::onCloseReminderFinished);
	connect(pPlugin, &UIPlugIn::fireUiDominationRequest, this, &AppController::onUiDominationRequested);
	connect(pPlugin, &UIPlugIn::fireUiDominationRelease, this, &AppController::onUiDominationRelease);
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
	Q_EMIT fireWorkflowStarted(mActiveWorkflow->getContext());

	if (!mActiveWorkflow->getContext()->wasClaimed())
	{
		qCritical() << "Workflow was not claimed by any UI... aborting";
		mActiveWorkflow->getContext()->killWorkflow(GlobalStatus::Code::Workflow_InternalError_BeforeTcToken);
		return false;
	}

	return true;
}


#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool AppController::nativeEventFilter(const QByteArray& pEventType, void* pMessage, qintptr* pResult)
#else
bool AppController::nativeEventFilter(const QByteArray& pEventType, void* pMessage, long* pResult)
#endif
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
			qDebug() << "WM_QUERYENDSESSION received";
			Q_EMIT fireHideUi();
		}
	}
#endif

	return false;
}


void AppController::clearCacheFolders()
{
	qDebug() << "Try to find and clear cache folder";

	const QStringList cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
	for (const QString& cachePath : cachePaths)
	{
		auto cacheDir = QDir(cachePath);

		if (!cacheDir.exists())
		{
			continue;
		}

		cacheDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
		auto entries = cacheDir.entryList();
		for (const QString& filePath : qAsConst(entries))
		{
			cacheDir.remove(filePath);
		}

		cacheDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
		entries = cacheDir.entryList();
		for (const QString& subDirPath : qAsConst(entries))
		{
			QDir subDir(cacheDir.absoluteFilePath(subDirPath));
			subDir.removeRecursively();
		}

		qDebug() << "Cleared cache folder:" << cachePath;
	}
}
