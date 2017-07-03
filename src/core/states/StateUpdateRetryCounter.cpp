/*!
 * StateUpdateRetryCounter.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "Result.h"
#include "StateUpdateRetryCounter.h"

using namespace governikus;


StateUpdateRetryCounter::StateUpdateRetryCounter(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateUpdateRetryCounter::run()
{
	qDebug() << "StateUpdateRetryCounter::run()";

	auto cardConnection = getContext()->getCardConnection();
	Q_ASSERT(cardConnection != nullptr);
	mConnections += cardConnection->callUpdateRetryCounterCommand(this, &StateUpdateRetryCounter::onUpdateRetryCounterDone);
}


void StateUpdateRetryCounter::onUpdateRetryCounterDone(QSharedPointer<BaseCardCommand> pCommand)
{
	qDebug() << "StateUpdateRetryCounter::onUpdateRetryCounterDone()";

	if (pCommand->getReturnCode() != CardReturnCode::OK)
	{
		qCritical() << "An error occurred while communicating with the card reader, cannot determine retry counter, abort state";
		setStatus(CardReturnCodeUtil::toGlobalStatus(pCommand->getReturnCode()));
		Q_EMIT fireError();
		return;
	}

	auto cardConnection = getContext()->getCardConnection();
	switch (cardConnection->getReaderInfo().getRetryCounter())
	{
		case 0:
			qDebug() << "PUK required";
			Q_EMIT fireRetryCounterIsZero();
			break;

		case 1:
			qDebug() << "CAN required";
			Q_EMIT fireRetryCounterIsOne();
			break;

		default:
			qDebug() << "PIN allowed";
			Q_EMIT fireRetryCounterIsGTOne();
	}
}
