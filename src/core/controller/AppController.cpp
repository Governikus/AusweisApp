/*!
 * AppController.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthenticationContext.h"
#include "controller/AppController.h"
#include "controller/AuthController.h"
#include "controller/ChangePinController.h"
#include "controller/SelfAuthController.h"
#include "DriverService.h"
#include "HttpServerRequestor.h"
#include "HttpServerStatusParser.h"
#include "LanguageLoader.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "ProviderService.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "view/UILoader.h"

#include <QCoreApplication>
#include <QSettings>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(support)


AppController::AppController()
	: mCurrentAction(Action::NONE)
	, mWaitingAction(Action::NONE)
	, mActiveController()
{
	setObjectName(QStringLiteral("AppController"));
	// read and apply the settings
	AppSettings::getInstance().load();

	LanguageLoader::getInstance().load();
	ResourceLoader::getInstance().init();

#ifndef QT_NO_NETWORKPROXY
	NetworkManager::setApplicationProxyFactory();
	connect(&NetworkManager::getGlobalInstance(), &NetworkManager::fireProxyAuthenticationRequired, this, &AppController::fireProxyAuthenticationRequired);
#endif

	connect(this, &AppController::fireShutdown, &NetworkManager::getGlobalInstance(), &NetworkManager::onShutdown, Qt::QueuedConnection);
}


AppController::~AppController()
{
}


bool AppController::start()
{
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

	ReaderManager::getInstance().init();
	connect(this, &AppController::fireShutdown, &ReaderManager::getInstance(), &ReaderManager::shutdown, Qt::DirectConnection);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireInitialized, this, &AppController::fireStarted, Qt::QueuedConnection);

	return true;
}


void AppController::onAuthenticationRequest(ActivationContext* pActivationContext)
{
	QSharedPointer<AuthContext> authContext(new AuthContext(pActivationContext));
#ifndef QT_NO_NETWORKPROXY
	connect(&authContext->getNetworkManager(), &NetworkManager::fireProxyAuthenticationRequired, this, &AppController::fireProxyAuthenticationRequired);
#endif

	if (!startNewWorkflow<AuthController>(Action::AUTH, authContext))
	{
		if (!pActivationContext->sendOperationAlreadyActive())
		{
			qCritical() << "Cannot send \"Operation already active\" to caller: " << pActivationContext->getSendError();
		}
	}
}


void AppController::onActiveControllerDone()
{
	qDebug() << mActiveController->metaObject()->className() << "done";
	disconnect(mActiveController.data(), &WorkflowController::fireComplete, this, &AppController::onActiveControllerDone);

	Q_EMIT fireWorkflowFinished(mActiveController->getContext());

	mActiveController.reset();
	qCInfo(support) << "Finished workflow" << mCurrentAction;
	mCurrentAction = Action::NONE;

	if (mWaitingAction != Action::NONE)
	{
		qDebug() << "Running waiting action now.";
		switch (mWaitingAction)
		{
			case Action::PIN:
				startNewWorkflow<ChangePinController>(Action::PIN,
					QSharedPointer<ChangePinContext>(new ChangePinContext));
				break;

			default:
				qWarning() << "Waiting action is unhandled!";
				break;
		}
		mWaitingAction = Action::NONE;

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
	if (!startNewWorkflow<ChangePinController>(Action::PIN,
			QSharedPointer<ChangePinContext>(new ChangePinContext)))
	{
		mWaitingAction = Action::PIN;
	}
}


void AppController::onSelfAuthenticationRequested()
{
	qDebug() << "self authentication requested";
	QSharedPointer<SelfAuthenticationContext> context(new SelfAuthenticationContext(nullptr));
#ifndef QT_NO_NETWORKPROXY
	connect(&context->getNetworkManager(), &NetworkManager::fireProxyAuthenticationRequired, this, &AppController::fireProxyAuthenticationRequired);
#endif
	startNewWorkflow<SelfAuthController>(Action::SELF, context);
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
			    // Only wait for connections created by the global network manager here. We assume,
			    // that each context has already cleaned up its own connections before discarding
			    // their NetworkManager instances.
				const int openConnectionCount = NetworkManager::getGlobalInstance().getOpenConnectionCount();
				static int timesInvoked = 0;
				static const int THREE_SECONDS = 3000;
				if (openConnectionCount > 0)
				{
					if (++timesInvoked % (THREE_SECONDS / TIMER_INTERVAL) == 0)
					{
						qDebug() << "There are still" << openConnectionCount << "pending network connections";
					}
					return;
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

#ifndef QT_NO_NETWORKPROXY
	connect(this, &AppController::fireProxyAuthenticationRequired, pPlugin, &UIPlugIn::onProxyAuthenticationRequired);
#endif

	connect(pPlugin, &UIPlugIn::fireChangePinRequest, this, &AppController::onChangePinRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireSelfAuthenticationRequested, this, &AppController::onSelfAuthenticationRequested, Qt::QueuedConnection);
	connect(pPlugin, &UIPlugIn::fireQuitApplicationRequest, this, &AppController::doShutdown);
	connect(pPlugin, &UIPlugIn::fireCloseReminderFinished, this, &AppController::onCloseReminderFinished);
}


template<typename Controller, typename Context>
bool AppController::startNewWorkflow(Action pAction, QSharedPointer<Context> pContext)
{
	// If a new authentication is requested and another authentication or self authentication is currently active, but the workflow is already finished
	// then the new authentication shall be started. So we deactivate the old workflow here.
	if (pAction == Action::AUTH && (mCurrentAction == Action::AUTH || mCurrentAction == Action::SELF) && mActiveController->getContext()->isWorkflowFinished())
	{
		onActiveControllerDone();
	}

	if (mCurrentAction != Action::NONE)
	{
		qWarning() << "Cannot start" << Controller::staticMetaObject.className() << "| Current action: " << mCurrentAction;
		return false;
	}
	Q_ASSERT(mActiveController.isNull());

	qCInfo(support) << "Starting new workflow" << pAction;

	mActiveController.reset(new Controller(pContext));
	mCurrentAction = pAction;
	connect(mActiveController.data(), &WorkflowController::fireComplete, this, &AppController::onActiveControllerDone, Qt::QueuedConnection);
	LogHandler::getInstance().resetBacklog();
	qDebug() << "Start" << mActiveController->metaObject()->className();

	//first: run new active controller so that it can be canceled during UI activation if required
	mActiveController->run();

	//second: activate ui
	Q_EMIT fireWorkflowStarted(pContext);

	return true;
}
