/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateStopIfdService.h"

#include "AppSettings.h"
#include "ReaderManager.h"

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

	// Request an asynchronous update of all retry counters
	Env::getSingleton<ReaderManager>()->updateRetryCounters();

	stopNfcScanIfNecessary();
	Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPlugInType::SMART);

	AbstractState::onExit(pEvent);
}
