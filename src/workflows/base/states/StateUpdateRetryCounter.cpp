/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
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
		Q_EMIT fireAbort(FailureCode::Reason::Update_Retry_Counter_No_Card_Connection);
		return;
	}

	Q_ASSERT(cardConnection != nullptr);
	mConnections += cardConnection->callUpdateRetryCounterCommand(this, &StateUpdateRetryCounter::onUpdateRetryCounterDone);
}


void StateUpdateRetryCounter::onUpdateRetryCounterDone(QSharedPointer<BaseCardCommand> pCommand)
{
	qCDebug(statemachine) << "StateUpdateRetryCounter::onUpdateRetryCounterDone()";

	const auto& returnCode = pCommand->getReturnCode();
	if (returnCode != CardReturnCode::OK)
	{
		qCCritical(statemachine) << "An error occurred while communicating with the card reader, cannot determine retry counter, abort state";
		getContext()->resetCardConnection();
		Q_EMIT fireAbort({FailureCode::Reason::Update_Retry_Counter_Communication_Error,
						  {FailureCode::Info::Card_Return_Code, Enum<CardReturnCode>::getName(returnCode)}
				});
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
