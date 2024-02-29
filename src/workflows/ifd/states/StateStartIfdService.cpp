/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateStartIfdService.h"

#include "AppSettings.h"
#include "ServerMessageHandler.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;


StateStartIfdService::StateStartIfdService(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateStartIfdService::run()
{
	QSharedPointer<IfdServiceContext> context = getContext();
	QSharedPointer<IfdServer> server = context->getIfdServer();
	Q_ASSERT(server);

	const auto& serverName = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getDeviceName();
	if (!server->isRunning() && !server->start(serverName))
	{
		qCCritical(statemachine) << "Cannot start IfdServer.";
		Q_EMIT fireAbort(FailureCode::Reason::Start_Ifd_Service_Failed);
		return;
	}

	Q_EMIT fireContinue();
}
