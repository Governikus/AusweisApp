/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

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
	ReaderManager::getInstance().stopScan();

	if (getContext()->getCardConnection())
	{
		qDebug() << "Going to disconnect card connection";
		getContext()->setCardConnection(QSharedPointer<CardConnection>());
	}

	qDebug() << "Going to disconnect readers";
	ReaderManager::getInstance().disconnectAllReaders();
	Q_EMIT fireSuccess();
}
