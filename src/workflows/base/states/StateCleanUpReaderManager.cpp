/*!
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StateCleanUpReaderManager.h"

#include "AppSettings.h"
#include "context/ChangePinContext.h"
#include "ReaderManager.h"
#include "VolatileSettings.h"

#include <QDebug>

using namespace governikus;

StateCleanUpReaderManager::StateCleanUpReaderManager(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateCleanUpReaderManager::run()
{
	const QSharedPointer<WorkflowContext> context = getContext();

	// Stop scanning is required to kill all connections to force deleting all pace passwords on a remote comfort reader
	const auto& status = context->getStatus();
	const auto readerManager = Env::getSingleton<ReaderManager>();
	readerManager->stopScanAll(status.isError() ? status.toErrorDescription(true) : QString());

#ifdef Q_OS_IOS
	readerManager->reset(ReaderManagerPlugInType::NFC);
#else
	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		readerManager->startScanAll();
	}
#endif

	if (context->getCardConnection())
	{
		qDebug() << "Going to disconnect card connection";
		context->resetCardConnection();
	}

	Q_EMIT fireContinue();
}
