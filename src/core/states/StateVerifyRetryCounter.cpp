/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateVerifyRetryCounter.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StateVerifyRetryCounter::StateVerifyRetryCounter(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateVerifyRetryCounter::run()
{
	const QSharedPointer<CardConnection>& cardConnection = getContext()->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "Card connection lost.";
		Q_EMIT fireAbort();
		return;
	}

	const int currentRetryCounter = cardConnection->getReaderInfo().getRetryCounter();
	qCDebug(statemachine) << "Retry counter | actual:" << currentRetryCounter << "/ expected:" << getContext()->getExpectedRetryCounter();

	if (!getContext()->isExpectedReader() || getContext()->getExpectedRetryCounter() != currentRetryCounter)
	{
		qCDebug(statemachine) << "The reader changed or the connected card has an unexpected retry counter. Clearing PACE passwords.";
		getContext()->resetPacePasswords();
		getContext()->rememberReader();
		getContext()->setExpectedRetryCounter(currentRetryCounter);
	}

	if (getContext()->getExpectedRetryCounter() == -1)
	{
		Q_ASSERT(currentRetryCounter != -1);

		qCDebug(statemachine) << "Remembering the selected reader and initializing the expected retry counter:" << currentRetryCounter;
		getContext()->rememberReader();
		getContext()->setExpectedRetryCounter(currentRetryCounter);
	}

	Q_EMIT fireContinue();
}
