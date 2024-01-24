/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateMaintainCardConnection.h"

#include "CardReturnCode.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


void StateMaintainCardConnection::handleWrongPacePassword()
{
	auto context = getContext();

	qCDebug(statemachine) << "Resetting all PACE passwords.";
	context->resetPacePasswords();

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
		Q_ASSERT(context->getFailureCode().has_value());
		Q_EMIT firePropagateAbort();
		return;
	}

	const CardReturnCode lastPaceResult = context->getLastPaceResult();
	qCDebug(statemachine) << "Last PACE result:" << lastPaceResult;

	switch (lastPaceResult)
	{
		case CardReturnCode::CANCELLATION_BY_USER:
		case CardReturnCode::PUK_INOPERATIVE:
		case CardReturnCode::INPUT_TIME_OUT:
		case CardReturnCode::UNKNOWN:
		case CardReturnCode::UNDEFINED:
		case CardReturnCode::COMMAND_FAILED:
		case CardReturnCode::PROTOCOL_ERROR:
		case CardReturnCode::WRONG_LENGTH:
		case CardReturnCode::UNEXPECTED_TRANSMIT_STATUS:
		{
			Q_ASSERT(!CardReturnCodeUtil::equalsWrongPacePassword(lastPaceResult));

			qCDebug(statemachine) << "Last PACE result is unrecoverable. Aborting.";
			updateStatus(CardReturnCodeUtil::toGlobalStatus(lastPaceResult));
			Q_EMIT fireAbort({FailureCode::Reason::Maintain_Card_Connection_Pace_Unrecoverable,
							  {FailureCode::Info::Card_Return_Code, Enum<CardReturnCode>::getName(lastPaceResult)}
					});
			return;
		}

		case CardReturnCode::INVALID_CAN:
		case CardReturnCode::INVALID_PASSWORD:
		case CardReturnCode::INVALID_PIN:
		case CardReturnCode::INVALID_PIN_2:
		case CardReturnCode::INVALID_PIN_3:
		case CardReturnCode::INVALID_PUK:
		case CardReturnCode::NEW_PIN_MISMATCH:
		case CardReturnCode::NEW_PIN_INVALID_LENGTH:
		case CardReturnCode::PIN_NOT_BLOCKED:
		case CardReturnCode::PIN_BLOCKED:
		{
			Q_ASSERT(CardReturnCodeUtil::equalsWrongPacePassword(lastPaceResult));

			handleWrongPacePassword();
			return;
		}

		case CardReturnCode::RETRY_ALLOWED:
		case CardReturnCode::CARD_NOT_FOUND:
		{
			Q_ASSERT(!CardReturnCodeUtil::equalsWrongPacePassword(lastPaceResult));

			qCDebug(statemachine) << "Assuming the card was removed (" << lastPaceResult << "). Resetting card connection and PACE result.";
			context->resetCardConnection();
			context->resetLastPaceResult();
			break;
		}

		case CardReturnCode::OK:
		case CardReturnCode::OK_PUK:
		{
			Q_ASSERT(!CardReturnCodeUtil::equalsWrongPacePassword(lastPaceResult));

			if (lastPaceResult == CardReturnCode::OK_PUK)
			{
				getContext()->resetLastPaceResult();

				qCDebug(statemachine) << "PIN unblocked! Triggering retry counter update.";
				Q_EMIT fireForceUpdateRetryCounter();
				return;
			}

			break;
		}
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
