/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
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
		Q_EMIT fireAbort();
		return;
	}

	const int currentRetryCounter = cardConnection->getReaderInfo().getRetryCounter();
	const int expectedRetryCounter = context->getExpectedRetryCounter();
	qCDebug(statemachine) << "Retry counter | actual:" << currentRetryCounter << "/ expected:" << expectedRetryCounter;

	if (context->remembersReader())
	{
		if (!context->isExpectedReader() || expectedRetryCounter != currentRetryCounter)
		{
			qCDebug(statemachine) << "The reader changed or the connected card has an unexpected retry counter. Clearing PACE passwords.";
			context->resetPacePasswords();
			context->rememberReader();
		}
	}
	else
	{
		Q_ASSERT(currentRetryCounter != -1);

		qCDebug(statemachine) << "Remembering the selected reader and initializing the expected retry counter:" << currentRetryCounter;
		context->rememberReader();
	}

	Q_EMIT fireContinue();
}
