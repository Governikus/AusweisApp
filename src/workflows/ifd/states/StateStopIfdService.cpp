/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#include "StateStopIfdService.h"

#include "ReaderManager.h"
#include "VolatileSettings.h"

using namespace governikus;


StateStopIfdService::StateStopIfdService(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateStopIfdService::run()
{
	Q_EMIT fireContinue();
}


void StateStopIfdService::onExit(QEvent* pEvent)
{
	// onExit: Stop if the user cancels the workflow, too.

	const QSharedPointer<IfdServer> server = getContext()->getIfdServer();
	Q_ASSERT(server);
	server->setPairing(false);
	server->stop();

	stopNfcScanIfNecessary();

	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPluginType::REMOTE_IFD);
	}

	AbstractState::onExit(pEvent);
}
