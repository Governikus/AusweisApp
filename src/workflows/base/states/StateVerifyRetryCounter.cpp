/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateVerifyRetryCounter.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StateVerifyRetryCounter::StateVerifyRetryCounter(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateVerifyRetryCounter::run()
{
	const auto& context = getContext();
	const QSharedPointer<CardConnection>& cardConnection = context->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "Card connection lost.";
		Q_EMIT fireNoCardConnection();
		return;
	}

	const int currentRetryCounter = cardConnection->getReaderInfo().getRetryCounter();
	const int expectedRetryCounter = context->getExpectedRetryCounter();
	qCDebug(statemachine) << "Retry counter | actual:" << currentRetryCounter << "/ expected:" << expectedRetryCounter;

	if (!context->remembersReader())
	{
		Q_ASSERT(currentRetryCounter != -1);

		qCDebug(statemachine) << "Initially remembering the selected reader including the card with retry counter" << currentRetryCounter;
		context->rememberReader();
		Q_EMIT fireContinue();
		return;
	}

	if (context->isExpectedReader() && expectedRetryCounter == currentRetryCounter)
	{
		qCDebug(statemachine) << "Found expected reader and retry counter matches";
		Q_EMIT fireContinue();
		return;
	}

	qCDebug(statemachine) << "The reader changed or the connected card has an unexpected retry counter. Remembering the new reader including the card with retry counter" << currentRetryCounter;
	context->rememberReader();
	Q_EMIT fireReaderOrCardChanged();
}
