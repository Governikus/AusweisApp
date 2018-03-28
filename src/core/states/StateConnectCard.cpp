/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "ReaderManager.h"
#include "Result.h"
#include "StateConnectCard.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;

StateConnectCard::StateConnectCard(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateConnectCard::run()
{
	qCDebug(statemachine) << "StateConnectCard::run()";
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StateConnectCard::onCardInserted);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StateConnectCard::onReaderRemoved);
	mConnections += connect(getContext().data(), &WorkflowContext::fireAbortCardSelection, this, &StateConnectCard::onAbort);
	mConnections += connect(getContext().data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &StateConnectCard::fireRetry);
	onCardInserted();
}


void StateConnectCard::onCardInserted()
{
	ReaderInfo readerInfo = ReaderManager::getInstance().getReaderInfo(getContext()->getReaderName());
	if (readerInfo.hasEidCard())
	{
		if (!readerInfo.isPinDeactivated())
		{
			qCDebug(statemachine) << "Card has been inserted, trying to connect";
			mConnections += ReaderManager::getInstance().callCreateCardConnectionCommand(readerInfo.getName(), this, &StateConnectCard::onCommandDone);
		}
	}
}


void StateConnectCard::onCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
{
	qCDebug(statemachine) << "Card connection command completed";
	if (pCommand->getCardConnection() == nullptr)
	{
		updateStatus(GlobalStatus::Code::Workflow_Reader_Communication_Error);
		Q_EMIT fireAbort();
		return;
	}

	qCDebug(statemachine) << "Card connection was successful";
	getContext()->setCardConnection(pCommand->getCardConnection());
	Q_EMIT fireContinue();
}


void StateConnectCard::onReaderRemoved(const QString& pReaderName)
{
	if (pReaderName == getContext()->getReaderName())
	{
		Q_EMIT fireReaderRemoved();
	}
}


void StateConnectCard::onAbort()
{
	const QSharedPointer<WorkflowContext> context = getContext();
	Q_ASSERT(context);

	ReaderManager::getInstance().stopScanAll();

	ReaderInfo readerInfo = ReaderManager::getInstance().getReaderInfo(context->getReaderName());
	if (readerInfo.isConnected())
	{
		ReaderManager::getInstance().disconnectReader(readerInfo.getName());
	}
	Q_EMIT fireRetry();
}
