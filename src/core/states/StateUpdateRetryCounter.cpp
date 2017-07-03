/*!
 * StateUpdateRetryCounter.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "Result.h"
#include "StateUpdateRetryCounter.h"

#include "ReturnCodeUtil.h"

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

	if (pCommand->getReturnCode() != ReturnCode::OK)
	{
		qCritical() << "An error occurred while communicating with the card reader, cannot determine retry counter, abort state";
		setResult(Result::createInternalError(ReturnCodeUtil::toMessage(pCommand->getReturnCode())));
		Q_EMIT fireError();
		return;
	}

	auto cardConnection = getContext()->getCardConnection();
	const int retryCounter = cardConnection->getReaderInfo().getRetryCounter();
	if (retryCounter == 0)
	{
		qDebug() << "Puk required";
		Q_EMIT fireRetryCounterIsZero();
	}
	else if (retryCounter == 1)
	{
		qDebug() << "Can required";
		Q_EMIT fireRetryCounterIsOne();
	}
	else
	{
		qDebug() << "Pin allowed";
		Q_EMIT fireRetryCounterIsGTOne();
	}
}
