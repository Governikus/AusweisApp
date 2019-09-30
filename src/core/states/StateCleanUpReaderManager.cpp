/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateCleanUpReaderManager.h"

#include "AppSettings.h"
#include "context/ChangePinContext.h"
#include "ReaderManager.h"

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
	const auto readerManager = Env::getSingleton<ReaderManager>();
	readerManager->stopScanAll();

	// On a desktop AusweisApp2, restart scanning when a change pin workflow is completed.
#if defined(Q_OS_WIN) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS)) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	const bool changePinOnDesktop = context.objectCast<ChangePinContext>();
#else
	const bool changePinOnDesktop = false;
#endif
	if (changePinOnDesktop || Env::getSingleton<AppSettings>()->isUsedAsSDK())
	{
		readerManager->startScanAll();
	}

	if (context->getCardConnection())
	{
		qDebug() << "Going to disconnect card connection";
		context->resetCardConnection();
	}

	Q_EMIT fireContinue();
}
