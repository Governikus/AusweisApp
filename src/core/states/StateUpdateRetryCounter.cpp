/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateUpdateRetryCounter.h"

#include "CardConnection.h"
#include "Result.h"


using namespace governikus;


StateUpdateRetryCounter::StateUpdateRetryCounter(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateUpdateRetryCounter::run()
{
	qDebug() << "StateUpdateRetryCounter::run()";

	auto cardConnection = getContext()->getCardConnection();
	if (cardConnection != nullptr)
	{
		mConnections += cardConnection->callUpdateRetryCounterCommand(this, &StateUpdateRetryCounter::onUpdateRetryCounterDone);
	}
	else
	{
		qDebug() << "Skipping update because there is no card connection";
		Q_EMIT fireContinue();
	}
}


void StateUpdateRetryCounter::onUpdateRetryCounterDone(QSharedPointer<BaseCardCommand> pCommand)
{
	qDebug() << "StateUpdateRetryCounter::onUpdateRetryCounterDone()";

	if (pCommand->getReturnCode() != CardReturnCode::OK)
	{
		qCritical() << "An error occurred while communicating with the card reader, cannot determine retry counter, abort state";
		updateStatus(CardReturnCodeUtil::toGlobalStatus(pCommand->getReturnCode()));
		Q_EMIT fireAbort();
		return;
	}

	Q_EMIT fireContinue();
}
