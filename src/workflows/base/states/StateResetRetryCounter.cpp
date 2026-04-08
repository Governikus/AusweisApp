/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateResetRetryCounter.h"

#include "apdu/ResponseApdu.h"

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
		qCDebug(statemachine) << "No card connection available";
		Q_EMIT fireNoCardConnection();
		return;
	}

	Q_ASSERT(cardConnection != nullptr);
	*this << cardConnection->callResetRetryCounterCommand(this, &StateResetRetryCounter::onResetRetryCounterDone);
}


void StateResetRetryCounter::onResetRetryCounterDone(QSharedPointer<BaseCardCommand> pCommand)
{
	auto resetRetryCounterCommand = pCommand.staticCast<ResetRetryCounterCommand>();
	const auto& returnCode = resetRetryCounterCommand->getReturnCode();
	if (returnCode == CardReturnCode::OK)
	{
		switch (const auto code = resetRetryCounterCommand->getStatusCode(); code)
		{
			case StatusCode::SUCCESS:
				getContext()->setExpectedRetryCounter(-1);
				Q_EMIT fireContinue();
				return;

			case StatusCode::ACCESS_DENIED:
				updateStatus(GlobalStatus::Code::Card_Puk_Blocked);
				Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_Puk_Inoperative);
				return;

			default:
				qCCritical(statemachine).nospace() << "Received an unexpected StatusCode (" << code << "), cannot reset retry counter";
		}
	}
	else
	{
		qCCritical(statemachine).nospace() << "An error (" << returnCode << ") occurred while communicating with the card reader";
	}

	getContext()->resetCardConnection();
	Q_EMIT fireNoCardConnection();
}
