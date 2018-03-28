/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "AppController.h"

#include "ActivationContext.h"
#include "AppSettings.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/RemoteServiceContext.h"
#include "context/SelfAuthContext.h"
#include "controller/AuthController.h"
#include "controller/ChangePinController.h"
#include "controller/RemoteServiceController.h"
#include "controller/SelfAuthController.h"
#include "Env.h"
#include "HttpServerRequestor.h"
#include "HttpServerStatusParser.h"
#include "LanguageLoader.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "view/UILoader.h"
#include "view/UIPlugIn.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QSettings>


namespace governikus
{
class WorkflowRequest
{
	private:
		const Action mAction;
		const QSharedPointer<WorkflowContext> mContext;

	public:
		WorkflowRequest(Action pAction, const QSharedPointer<WorkflowContext>& pContext);
		~WorkflowRequest() = default;

		inline Action getAction() const;
		inline QSharedPointer<WorkflowContext> getContext() const;
};

}


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


bool AppController::canStartNewAction()
{
	return mCurrentAction == Action::NONE && mActiveController.isNull();
}


AppController::AppController()
	: mCurrentAction(Action::NONE)
	, mWaitingRequest()
	, mActiveController()
{
	setObjectName(QStringLiteral("AppController"));

	connect(&AppSettings::getInstance().getGeneralSettings(), &GeneralSettings::fireSettingsChanged, this, &AppController::onSettingsChanged, Qt::DirectConnection);
	onSettingsChanged();

	ResourceLoader::getInstance().init();

#ifndef QT_NO_NETWORKPROXY
	NetworkManager::setApplicationProxyFactory();
	connect(Env::getSingleton<NetworkManager>(), &NetworkManager::fireProxyAuthenticationRequired, this, &AppController::fireProxyAuthenticationRequired);
#endif

	connect(this, &AppController::fireShutdown, Env::getSingleton<NetworkManager>(), &NetworkManager::onShutdown, Qt::QueuedConnection);
}


AppController::~AppController()
{
}


bool AppController::eventFilter(QObject* pObj, QEvent* pEvent)
{
#ifdef Q_OS_MACOS
	// This event gets send on reopen events in macOS (opening the app again while
	// it is in the background). Currently the event handling is only needed on macOS.
	if (pEvent && pEvent->type() == QEvent::ApplicationActivate)
	{
		qCDebug(system) << "Got an ApplicationActivate event, showing current UI Workflow.";
		Q_EMIT fireShowUi(UiModule::CURRENT);
	}
#endif
	return QObject::eventFilter(pObj, pEvent);
}


bool AppController::start()
{
	ReaderManager::getInstance().init(QSharedPointer<RemoteClient>(Env::create<RemoteClient*>()));
	connect(this, &AppController::fireShutdown, &ReaderManager::getInstance(), &ReaderManager::shutdown, Qt::DirectConnection);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireInitialized, this, &AppController::fireStarted, Qt::QueuedConnection);

	connect(&UILoader::getInstance(), &UILoader::fireLoadedPlugin, this, &AppController::onUiPlugin);
	if (!UILoader::getInstance().load())
	{
		qCritical() << "Cannot start without UI";
		return false;
	}

	for (const auto& handler : ActivationHandler::getInstances())
	{
		connect(handler, &ActivationHandler::fireShowUserInformation, this, &AppController::fireShowUserInformation);
		connect(handler, &ActivationHandler::fireShowUiRequest, this, &AppController::fireShowUi);
		connect(handler, &ActivationHandler::fireAuthenticationRequest, this, &AppController::onAuthenticationRequest);

		if (!handler->start())
		{
			qCritical() << "Cannot start activation handler:" << handler;
			doShutdown();
			return false;
		}
		qDebug() << "Successfully started activation handler:" << handler;
	}

	QCoreApplication::instance()->installEventFilter(this);

	return true;
}


void AppController::onWorkflowFinished()
{
	qDebug() << mActiveController->metaObject()->className() << "done";
	disconnect(mActiveController.data(), &WorkflowController::fireComplete, this, &AppController::onWorkflowFinished);

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
			}
			break;

			case Action::AUTH:
			{
				const QSharedPointer<AuthContext> context = mWaitingRequest->getContext().objectCast<AuthContext>();
				Q_ASSERT(context);
				startNewWorkflow<AuthController>(Action::AUTH, context);
			}
			break;

			case Action::READER_SETTINGS:
			{
				Q_EMIT fireShowReaderSettings();
			}
			break;

			default:
				qWarning() << "Waiting action is unhandled!";
				break;
		}

		mWaitingRequest.reset();
	}
}


void AppController::onCloseReminderFinished(bool pDontRemindAgain)
{
	if (pDontRemindAgain)
	{
		AppSettings::getInstance().getGeneralSettings().setRemindUserToClose(false);
		AppSettings::getInstance().getGeneralSettings().save();
	}
}


void AppController::onChangePinRequested()
{
	qDebug() << "PIN change requested";
	const QSharedPointer<ChangePinContext> context(new ChangePinContext);
	if (canStartNewAction())
	{
		startNewWorkflow<ChangePinController>(Action::PIN, context);

		return;
	}

	if (mWaitingRequest.isNull())
	{
		mWaitingRequest.reset(new WorkflowRequest(Action::PIN, context));

		return;
	}

	qWarning() << "Cannot enqueue action" << Action::PIN << ", queue is already full.";
}


void AppController::onSwitchToReaderSettingsRequested()
{
	if (canStartNewAction())
	{
		Q_EMIT fireShowReaderSettings();

		return;
	}

	if (mWaitingRequest.isNull())
	{
		mWaitingRequest.reset(new WorkflowRequest(Action::READER_SETTINGS, QSharedPointer<WorkflowContext>()));

		return;
	}

	qWarning() << "Cannot enqueue action" << Action::READER_SETTINGS << ", queue is already full.";
}


void AppController::onSelfAuthenticationRequested()
{
	qDebug() << "self authentication requested";
	if (canStartNewAction())
	{
		const QSharedPointer<SelfAuthContext> context(new SelfAuthContext());
		startNewWorkflow<SelfAuthController>(Action::SELF, context);
	}
}


void AppController::onAuthenticationRequest(const QSharedPointer<ActivationContext>& pActivationContext)
{
	qDebug() << "authentication requested";
	const QSharedPointer<AuthContext> authContext(new AuthContext(pActivationContext));
	if (canStartNewAction())
	{
		startNewWorkflow<AuthController>(Action::AUTH, authContext);
		return;
	}

	if (mCurrentAction == Action::SELF)
	{
		const QSharedPointer<WorkflowContext> activeContext = mActiveController->getContext();
		Q_ASSERT(!activeContext.isNull());
		if (activeContext->isWorkflowFinished())
		{
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
		qCritical() << "Cannot send \"Operation already active\" to caller: " << pActivationContext->getSendError();
	}
}


void AppController::onRemoteServiceRequested()
{
	qDebug() << "remote service requested";
	if (canStartNewAction())
	{
		const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
		startNewWorkflow<RemoteServiceController>(Action::REMOTE_SERVICE, context);
	}
}


void AppController::onSettingsChanged()
{
	LanguageLoader& languageLoader = LanguageLoader::getInstance();
	const QLocale& newLocale = QLocale(AppSettings::getInstance().getGeneralSettings().getLanguage());
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
}


void AppController::doShutdown()
{
	qDebug() << "Emit fire shutdown";
	Q_EMIT fireShutdown();
	ResourceLoader::getInstance().shutdown();

	QTimer* timer = new QTimer();
	static const int TIMER_INTERVAL = 50;
	timer->setInterval(TIMER_INTERVAL);
	connect(timer, &QTimer::timeout, [ = ](){
				// Only wait for connections created by the global network manager here. We assume
				// that each context has already cleaned up its own connections before discarding
				// their NetworkManager instances.
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
				}

				timer->deleteLater();
				qDebug() << "Quit event loop of QCoreApplication";
				qApp->quit();
			});
	timer->start();
}


void AppController::onUiPlugin(UIPlugIn* pPlugin)
{
	qDebug() << "Register UI:" << pPlugin->metaObject()->className();
	connect(this, &AppController::fireShutdown, pPlugin, &UIPlugIn::doShutdown, Qt::DirectConnection);
	connect(this, &AppController::fireWorkflowStarted, pPlugin, &UIPlugIn::onWorkflowStarted);
	connect(this, &AppController::fireWorkflowFinished, pPlugin, &UIPlugIn::onWorkflowFinished);
	connect(this, &AppController::fireStarted, pPlugin, &UIPlugIn::onApplicationStarted);
	connect(this, &AppController::fireShowUi, pPlugin, &UIPlugIn::onShowUi);
	connect(this, &AppController::fireShowUserInformation, pPlugin, &UIPlugIn::fireShowUserInformation);
	connect(this, &AppController::fireShowReaderSettings, pPlugin, &UIPlugIn::onShowReaderSettings);

#ifndef QT_NO_NETWORKPROXY
	connect(this, &AppController::fireProxyAuthenticationRequired, pPlugin, &UIPlugIn::onProxyAuthenticationRequired);
#endif

	connect(pPlugin, &UIPlugIn::fireChangePinRequest, this, &AppController::onChangePinRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireSwitchToReaderSettingsRequested, this, &AppController::onSwitchToReaderSettingsRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireSelfAuthenticationRequested, this, &AppController::onSelfAuthenticationRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireRemoteServiceRequested, this, &AppController::onRemoteServiceRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireQuitApplicationRequest, this, &AppController::doShutdown);
	connect(pPlugin, &UIPlugIn::fireCloseReminderFinished, this, &AppController::onCloseReminderFinished);
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
	LogHandler::getInstance().resetBacklog();
	qDebug() << "Start" << mActiveController->metaObject()->className();

	//first: run new active controller so that it can be canceled during UI activation if required
	mActiveController->run();

	//second: activate ui
	Q_EMIT fireWorkflowStarted(pContext);

	return true;
}
