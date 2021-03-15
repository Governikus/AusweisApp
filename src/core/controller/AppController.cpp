/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "AppController.h"

#include "AppSettings.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"
#include "controller/AuthController.h"
#include "controller/ChangePinController.h"
#include "controller/SelfAuthController.h"
#include "InternalActivationContext.h"
#include "LanguageLoader.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "SecureStorage.h"
#include "UILoader.h"
#include "UIPlugIn.h"

#if __has_include("RemoteClient.h")
	#include "context/RemoteServiceContext.h"
	#include "controller/RemoteServiceController.h"
	#include "RemoteClient.h"
#endif

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

class WorkflowRequest final
{
	private:
		const Action mAction;
		const QSharedPointer<WorkflowContext> mContext;

	public:
		WorkflowRequest(Action pAction, const QSharedPointer<WorkflowContext>& pContext);
		~WorkflowRequest() = default;

		[[nodiscard]] inline Action getAction() const;
		[[nodiscard]] inline QSharedPointer<WorkflowContext> getContext() const;
};

} // namespace governikus


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(support)
Q_DECLARE_LOGGING_CATEGORY(system)


WorkflowRequest::WorkflowRequest(Action pAction,
		const QSharedPointer<WorkflowContext>& pContext)
	: mAction(pAction)
	, mContext(pContext)
{
}


Action WorkflowRequest::getAction() const
{
	return mAction;
}


QSharedPointer<WorkflowContext> WorkflowRequest::getContext() const
{
	return mContext;
}


bool AppController::canStartNewAction() const
{
	return mCurrentAction == Action::NONE && mActiveController.isNull();
}


AppController::AppController()
	: mCurrentAction(Action::NONE)
	, mWaitingRequest()
	, mActiveController()
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

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireLanguageChanged, this, &AppController::onLanguageChanged, Qt::DirectConnection);
	onLanguageChanged();

	ResourceLoader::getInstance().init();

	NetworkManager::setApplicationProxyFactory();
	connect(Env::getSingleton<NetworkManager>(), &NetworkManager::fireProxyAuthenticationRequired, this, &AppController::fireProxyAuthenticationRequired);

	connect(this, &AppController::fireShutdown, Env::getSingleton<NetworkManager>(), &NetworkManager::onShutdown, Qt::QueuedConnection);
}


AppController::~AppController()
{
	LanguageLoader::getInstance().unload();
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

#if __has_include("RemoteClient.h")
	// Force construction of RemoteClient in the MainThread
	Env::getSingleton<RemoteClient>();
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
			qCritical() << "Cannot start activation handler:" << handler;
			doShutdown();
			return false;
		}
		qDebug() << "Successfully started activation handler:" << handler;
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
	Q_ASSERT(mActiveController);

	qDebug() << mActiveController->metaObject()->className() << "done";
	disconnect(mActiveController.data(), &WorkflowController::fireComplete, this, &AppController::onWorkflowFinished);

	const auto authContext = mActiveController->getContext().objectCast<AuthContext>();
	if (authContext)
	{
		if (authContext->getLastPaceResult() == CardReturnCode::NO_ACTIVE_PIN_SET)
		{
			onChangePinRequested(true);
		}
	}

	Q_EMIT fireWorkflowFinished(mActiveController->getContext());

	mActiveController.reset();
	qCInfo(support) << "Finished workflow" << mCurrentAction;
	mCurrentAction = Action::NONE;

	if (!mWaitingRequest.isNull())
	{
		qDebug() << "Running waiting action now.";
		switch (mWaitingRequest->getAction())
		{
			case Action::PIN:
			{
				const QSharedPointer<ChangePinContext> context = mWaitingRequest->getContext().objectCast<ChangePinContext>();
				Q_ASSERT(context);
				startNewWorkflow<ChangePinController>(Action::PIN, context);
				break;
			}

			case Action::AUTH:
			{
				const QSharedPointer<AuthContext> context = mWaitingRequest->getContext().objectCast<AuthContext>();
				Q_ASSERT(context);
				startNewWorkflow<AuthController>(Action::AUTH, context);
				break;
			}

			case Action::READER_SETTINGS:
			{
				Q_EMIT fireShowReaderSettings();
				break;
			}

			default:
				qWarning() << "Waiting action is unhandled!";
				break;
		}

		mWaitingRequest.reset();
	}

	if (mShutdownRunning)
	{
		completeShutdown();
	}
}


void AppController::onCloseReminderFinished(bool pDontRemindAgain)
{
	if (pDontRemindAgain)
	{
		Env::getSingleton<AppSettings>()->getGeneralSettings().setRemindUserToClose(false);
		Env::getSingleton<AppSettings>()->getGeneralSettings().save();
	}
}


void AppController::onChangePinRequested(bool pRequestTransportPin)
{
	if (canStartNewAction())
	{
		qDebug() << "PIN change requested";
		const auto& context = QSharedPointer<ChangePinContext>::create(pRequestTransportPin);
		startNewWorkflow<ChangePinController>(Action::PIN, context);

		return;
	}

	if (mWaitingRequest.isNull())
	{
		qDebug() << "PIN change enqueued";
		mActiveController->getContext()->setNextWorkflowPending(true);
		const auto& context = QSharedPointer<ChangePinContext>::create(pRequestTransportPin);
		mWaitingRequest.reset(new WorkflowRequest(Action::PIN, context));

		return;
	}

	qWarning() << "Cannot enqueue action" << Action::PIN << ", queue is already full.";
}


void AppController::onSelfAuthenticationRequested()
{
	qDebug() << "Self-authentication requested";
	if (canStartNewAction())
	{
		const auto& context = QSharedPointer<SelfAuthContext>::create();
		startNewWorkflow<SelfAuthController>(Action::SELF, context);
	}
}


void AppController::onAuthenticationRequest(const QUrl& pUrl)
{
	onAuthenticationContextRequest(QSharedPointer<InternalActivationContext>::create(pUrl));
}


void AppController::onAuthenticationContextRequest(const QSharedPointer<ActivationContext>& pActivationContext)
{
	qDebug() << "Authentication requested";
	const auto& authContext = QSharedPointer<AuthContext>::create(pActivationContext);
	if (canStartNewAction())
	{
		startNewWorkflow<AuthController>(Action::AUTH, authContext);
		return;
	}

	if (mCurrentAction == Action::AUTH || mCurrentAction == Action::SELF || mCurrentAction == Action::PIN)
	{
		const QSharedPointer<WorkflowContext> activeContext = mActiveController->getContext();
		Q_ASSERT(!activeContext.isNull());
		if (activeContext->isWorkflowFinished())
		{
			qDebug() << "Auto-approving the current state";
			if (mWaitingRequest.isNull())
			{
				mWaitingRequest.reset(new WorkflowRequest(Action::AUTH, authContext));
				activeContext->setStateApproved(true);

				return;
			}

			qWarning() << "Cannot enqueue action" << Action::AUTH << ", queue is already full.";
		}
	}

	if (!pActivationContext->sendOperationAlreadyActive())
	{
		qCritical() << "Cannot send \"Operation already active\" to caller:" << pActivationContext->getSendError();
	}
}


void AppController::onRemoteServiceRequested()
{
	qDebug() << "remote service requested";

#if __has_include("RemoteClient.h")
	if (canStartNewAction())
	{
		const auto& context = QSharedPointer<RemoteServiceContext>::create();
		startNewWorkflow<RemoteServiceController>(Action::REMOTE_SERVICE, context);
	}
#endif
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


void AppController::doShutdown()
{
	doShutdown(EXIT_SUCCESS);
}


void AppController::doShutdown(int pExitCode)
{
	mExitCode = pExitCode;
	mShutdownRunning = true;
	mActivationController.shutdown();

	if (mActiveController)
	{
		// Make sure that any request for a new workflow is removed from the queue.
		mWaitingRequest.reset();

		// Make sure running workflow is not blocked by any open dialogs
		// (hiding the GUI also closes open dialogs).
		Q_EMIT fireHideUi();

		// Make sure the workflow runs to the end without user interaction.
		const QSharedPointer<WorkflowContext> context = mActiveController->getContext();
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
	if (mUiDomination == nullptr && mCurrentAction == Action::NONE)
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


void AppController::onUiPlugin(UIPlugIn* pPlugin)
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
	connect(this, &AppController::fireShowReaderSettings, pPlugin, &UIPlugIn::onShowReaderSettings);
	connect(this, &AppController::fireApplicationActivated, pPlugin, &UIPlugIn::fireApplicationActivated);
	connect(this, &AppController::fireUiDomination, pPlugin, &UIPlugIn::onUiDomination);
	connect(this, &AppController::fireUiDominationReleased, pPlugin, &UIPlugIn::onUiDominationReleased);
	connect(this, &AppController::fireProxyAuthenticationRequired, pPlugin, &UIPlugIn::onProxyAuthenticationRequired);

	connect(pPlugin, &UIPlugIn::fireChangePinRequested, this, &AppController::onChangePinRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireSelfAuthenticationRequested, this, &AppController::onSelfAuthenticationRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireAuthenticationRequest, this, &AppController::onAuthenticationRequest, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireRemoteServiceRequested, this, &AppController::onRemoteServiceRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireRestartApplicationRequested, this, &AppController::onRestartApplicationRequested, Qt::QueuedConnection);
	connect(pPlugin, qOverload<>(&UIPlugIn::fireQuitApplicationRequest), this, qOverload<>(&AppController::doShutdown));
	connect(pPlugin, qOverload<int>(&UIPlugIn::fireQuitApplicationRequest), this, qOverload<int>(&AppController::doShutdown));
	connect(pPlugin, &UIPlugIn::fireCloseReminderFinished, this, &AppController::onCloseReminderFinished);
	connect(pPlugin, &UIPlugIn::fireUiDominationRequest, this, &AppController::onUiDominationRequested);
	connect(pPlugin, &UIPlugIn::fireUiDominationRelease, this, &AppController::onUiDominationRelease);
}


template<typename Controller, typename Context>
bool AppController::startNewWorkflow(Action pAction, const QSharedPointer<Context>& pContext)
{
	if (mCurrentAction != Action::NONE || !mActiveController.isNull())
	{
		qWarning() << "Cannot start" << Controller::staticMetaObject.className() << "| Current action: " << mCurrentAction;
		return false;
	}

	qCInfo(support) << "Starting new workflow" << pAction;

	mActiveController.reset(new Controller(pContext));
	mCurrentAction = pAction;
	connect(mActiveController.data(), &WorkflowController::fireComplete, this, &AppController::onWorkflowFinished, Qt::QueuedConnection);
	Env::getSingleton<LogHandler>()->resetBacklog();
	qDebug() << "Start" << mActiveController->metaObject()->className();

	//first: run new active controller so that it can be canceled during UI activation if required
	mActiveController->run();

	//second: activate ui
	Q_EMIT fireWorkflowStarted(pContext);

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
