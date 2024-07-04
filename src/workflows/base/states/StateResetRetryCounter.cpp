/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateResetRetryCounter.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;

StateResetRetryCounter::StateResetRetryCounter(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateResetRetryCounter::run()
{
	const auto& context = getContext();
	Q_ASSERT(context);

	auto cardConnection = getContext()->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "No card connection available.";
		Q_EMIT fireNoCardConnection();
		return;
	}

	Q_ASSERT(cardConnection != nullptr);
	*this << cardConnection->callResetRetryCounterCommand(this, &StateResetRetryCounter::onResetRetryCounterDone);
}


void StateResetRetryCounter::onResetRetryCounterDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const auto& returnCode = pCommand->getReturnCode();
	switch (returnCode)
	{
		case CardReturnCode::OK:
			getContext()->setExpectedRetryCounter(-1);
			Q_EMIT fireContinue();
			return;

		case CardReturnCode::PUK_INOPERATIVE:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_Puk_Inoperative);
			return;

		default:
			qCCritical(statemachine).nospace() << "An error (" << returnCode << ") occurred while communicating with the card reader, cannot reset retry counter, abort state";
			getContext()->resetCardConnection();
			Q_EMIT fireNoCardConnection();
			return;

	}

	Q_UNREACHABLE();
}
