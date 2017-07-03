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
	mConnections += connect(getContext().data(), &WorkflowContext::fireAbortCardSelection, this, &StateConnectCard::fireAbort);
	onCardInserted();
}


void StateConnectCard::onCardInserted()
{
	ReaderInfo readerInfo = ReaderManager::getInstance().getReaderInfo(getContext()->getReaderName());
	if (readerInfo.getCardType() == CardType::EID_CARD)
	{
		qDebug() << "Card has been inserted, trying to connect";
		mConnections += ReaderManager::getInstance().callCreateCardConnectionCommand(readerInfo.getName(), this, &StateConnectCard::onCommandDone);
	}
}


void StateConnectCard::onCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
{
	qDebug() << "Card connection command completed";
	if (pCommand->getCardConnection() == nullptr)
	{
		error(tr("An error occurred while communicating with the card reader."));
		return;
	}

	qDebug() << "Card connection was successful";
	getContext()->setCardConnection(pCommand->getCardConnection());
	Q_EMIT fireSuccess();
}


void StateConnectCard::error(const QString& pText)
{
	setResult(Result::createCommunicationError(pText));
	Q_EMIT fireError();
}


void StateConnectCard::onReaderRemoved(const QString& pReaderName)
{
	if (pReaderName == getContext()->getReaderName())
	{
		Q_EMIT fireReaderRemoved();
	}
}
