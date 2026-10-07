/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#include "StateMaintainCardConnection.h"

#include "CardReturnCode.h"


Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


void StateMaintainCardConnection::handleWrongPacePassword()
{
	auto context = getContext();

	if (context->getCardConnection())
	{
		qCDebug(statemachine) << "Trigger retry counter update.";
		Q_EMIT fireForceUpdateRetryCounter();
	}
	else
	{
		qCDebug(statemachine) << "No card connection available.";
		Q_EMIT fireNoCardConnection();
	}
}


StateMaintainCardConnection::StateMaintainCardConnection(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateMaintainCardConnection::run()
{
	auto context = getContext();
	if (context->getStatus().isError())
	{
		auto failure = context->getFailureCode();
		if (!failure.has_value())
		{
			failure = FailureCode::Reason::Maintain_Card_Connection_Unknown_Error;
		}
		Q_EMIT fireAbort(failure.value());
		return;
	}

	const auto& paceOutput = context->getPaceOutput();
	const CardReturnCode lastPaceResult = paceOutput.getReturnCode();
	qCDebug(statemachine) << "Last PACE result:" << lastPaceResult;

	switch (lastPaceResult)
	{
		case CardReturnCode::CANCELLATION_BY_USER:
		case CardReturnCode::INPUT_TIME_OUT:
		case CardReturnCode::UNKNOWN:
		case CardReturnCode::COMMAND_FAILED:
		case CardReturnCode::PROTOCOL_ERROR:
		case CardReturnCode::WRONG_LENGTH:
		case CardReturnCode::UNEXPECTED_TRANSMIT_STATUS:
		{
			qCDebug(statemachine) << "Last PACE result is unrecoverable. Aborting.";
			updateStatus(CardReturnCodeUtil::toGlobalStatus(lastPaceResult));
			Q_EMIT fireAbort({FailureCode::Reason::Maintain_Card_Connection_Pace_Unrecoverable,
							  {FailureCode::Info::Card_Return_Code, Enum<CardReturnCode>::getName(lastPaceResult)}
					});
			return;
		}

		case CardReturnCode::PIN_NOT_BLOCKED:
		{
			handleWrongPacePassword();
			return;
		}

		case CardReturnCode::RESPONSE_EMPTY:
		case CardReturnCode::CARD_NOT_FOUND:
		{
			qCDebug(statemachine) << "Assuming the card was removed (" << lastPaceResult << "). Resetting card connection and PACE result.";
			context->resetCardConnection();
			context->resetPaceOutput();
			break;
		}

		case CardReturnCode::OK:
			if (paceOutput.wrongPasswordUsed())
			{
				handleWrongPacePassword();
				return;
			}

			if (paceOutput.getPaceResult() == PaceResult::OK_PUK && context->getCardConnection())
			{
				qCDebug(statemachine) << "PIN unblocked! Triggering retry counter update.";
				Q_EMIT fireForceUpdateRetryCounter();
				return;
			}

			break;

		case CardReturnCode::UNDEFINED:
			break;
	}

	if (!context->getCardConnection())
	{
		qCDebug(statemachine) << "No card connection available.";
		Q_EMIT fireNoCardConnection();
		return;
	}

	qCDebug(statemachine) << "Card connection is fine. Proceeding.";
	Q_EMIT fireContinue();
}
