/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "context/ChangePinContext.h"
#include "ReaderManager.h"
#include "StateCleanUpReaderManager.h"

#include <QDebug>

using namespace governikus;

StateCleanUpReaderManager::StateCleanUpReaderManager(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateCleanUpReaderManager::run()
{
	const QSharedPointer<WorkflowContext> context = getContext();

	// On a stationary AusweisApp2, do not stop scanning when a change pin workflow is completed.
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) || defined(Q_OS_FREEBSD)
	const bool stopScanRequired = context.objectCast<ChangePinContext>().isNull();
#else
	const bool stopScanRequired = true;
#endif

	if (stopScanRequired)
	{
		ReaderManager::getInstance().stopScanAll();
	}

	if (context->getCardConnection())
	{
		qDebug() << "Going to disconnect card connection";
		context->setCardConnection(QSharedPointer<CardConnection>());
	}

	qDebug() << "Going to disconnect readers";
	ReaderManager::getInstance().disconnectAllReaders();
	Q_EMIT fireContinue();
}
