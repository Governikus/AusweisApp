/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateStartRemoteService.h"

#include "AppSettings.h"
#include "ServerMessageHandler.h"

using namespace governikus;


StateStartRemoteService::StateStartRemoteService(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


StateStartRemoteService::~StateStartRemoteService()
{
}


void StateStartRemoteService::run()
{
	QSharedPointer<RemoteServiceContext> context = getContext();
	QSharedPointer<RemoteServer> server = context->getRemoteServer();
	Q_ASSERT(server);

	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	server->start(settings.getServerName());

	connect(server.data(), &RemoteServer::fireMessageHandlerAdded, this, &StateStartRemoteService::onMessageHandlerAdded);

	Q_EMIT fireContinue();
}


void StateStartRemoteService::onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler)
{
	const QSharedPointer<RemoteServiceContext> context = getContext();
	connect(pHandler.data(), &ServerMessageHandler::fireClosed, context.data(), &RemoteServiceContext::onResetMessageHandler, Qt::QueuedConnection);
}
