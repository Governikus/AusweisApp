/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "ReaderManager.h"
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
	const auto readerManager = Env::getSingleton<ReaderManager>();
	mConnections += connect(readerManager, &ReaderManager::fireCardInserted, this, &StateConnectCard::onCardInserted);
	mConnections += connect(readerManager, &ReaderManager::fireReaderRemoved, this, &StateConnectCard::onReaderRemoved);
	mConnections += connect(getContext().data(), &WorkflowContext::fireAbortCardSelection, this, &StateConnectCard::onAbort);
	mConnections += connect(getContext().data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &StateConnectCard::fireRetry);
	onCardInserted();
}


void StateConnectCard::onCardInserted()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	ReaderInfo readerInfo = readerManager->getReaderInfo(getContext()->getReaderName());
	if (readerInfo.hasEidCard())
	{
		if (readerInfo.sufficientApduLength() && (!readerInfo.isPinDeactivated() || getContext()->isCanAllowedMode()))
		{
			qCDebug(statemachine) << "Card has been inserted, trying to connect";
			mConnections += readerManager->callCreateCardConnectionCommand(readerInfo.getName(), this, &StateConnectCard::onCommandDone);
		}
	}
}


void StateConnectCard::onCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
{
	qCDebug(statemachine) << "Card connection command completed";
	if (pCommand->getCardConnection() == nullptr)
	{
		qCDebug(statemachine) << "Card connection failed";
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

	const auto readerManager = Env::getSingleton<ReaderManager>();
	ReaderInfo readerInfo = readerManager->getReaderInfo(context->getReaderName());
	if (readerInfo.isConnected())
	{
		readerManager->disconnectReader(readerInfo.getName());
	}
	Q_EMIT fireRetry();
}
