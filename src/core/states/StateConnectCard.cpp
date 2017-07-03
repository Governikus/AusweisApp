/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "ReaderManager.h"
#include "Result.h"
#include "StateConnectCard.h"

using namespace governikus;

StateConnectCard::StateConnectCard(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateConnectCard::run()
{
	qDebug() << "StateConnectCard::run()";
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StateConnectCard::onCardInserted);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StateConnectCard::onReaderRemoved);
	mConnections += connect(getContext().data(), &WorkflowContext::fireAbortCardSelection, this, &StateConnectCard::onAbort);
	onCardInserted();
}


void StateConnectCard::onCardInserted()
{
	ReaderInfo readerInfo = ReaderManager::getInstance().getReaderInfo(getContext()->getReaderName());
	if (readerInfo.getCardType() == CardType::EID_CARD)
	{
		if (!readerInfo.isPinDeactivated())
		{
			qDebug() << "Card has been inserted, trying to connect";
			mConnections += ReaderManager::getInstance().callCreateCardConnectionCommand(readerInfo.getName(), this, &StateConnectCard::onCommandDone);
		}
	}
}


void StateConnectCard::onCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
{
	qDebug() << "Card connection command completed";
	if (pCommand->getCardConnection() == nullptr)
	{
		setStatus(GlobalStatus::Code::Workflow_Reader_Communication_Error);
		Q_EMIT fireError();
		return;
	}

	qDebug() << "Card connection was successful";
	getContext()->setCardConnection(pCommand->getCardConnection());
	Q_EMIT fireSuccess();
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
	ReaderInfo readerInfo = ReaderManager::getInstance().getReaderInfo(getContext()->getReaderName());
	if (readerInfo.isValid() && readerInfo.isConnected())
	{
		ReaderManager::getInstance().disconnectReader(readerInfo.getName());
	}
	Q_EMIT fireAbort();
}
