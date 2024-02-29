/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateUpdateRetryCounter.h"

#include "CardConnection.h"

#include <QDebug>

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;


StateUpdateRetryCounter::StateUpdateRetryCounter(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateUpdateRetryCounter::run()
{
	auto cardConnection = getContext()->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "No card connection available.";
		Q_EMIT fireNoCardConnection();
		return;
	}

	Q_ASSERT(cardConnection != nullptr);
	*this << cardConnection->callUpdateRetryCounterCommand(this, &StateUpdateRetryCounter::onUpdateRetryCounterDone);
}


void StateUpdateRetryCounter::onUpdateRetryCounterDone(QSharedPointer<BaseCardCommand> pCommand)
{
	qCDebug(statemachine) << "StateUpdateRetryCounter::onUpdateRetryCounterDone()";

	const auto& returnCode = pCommand->getReturnCode();
	if (returnCode != CardReturnCode::OK)
	{
		qCCritical(statemachine).nospace() << "An error (" << returnCode << ") occurred while communicating with the card reader, cannot determine retry counter, abort state";
		getContext()->resetCardConnection();
		Q_EMIT fireNoCardConnection();
		return;
	}

#ifndef QT_NO_DEBUG
	if (getContext()->getCardConnection()->getReaderInfo().getRetryCounter() == -1)
	{
		qCWarning(statemachine) << "Retry counter should be initialized if command has succeeded.";
	}
#endif

	Q_EMIT fireContinue();
}
