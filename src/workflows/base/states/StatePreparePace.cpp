/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StatePreparePace.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StatePreparePace::StatePreparePace(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StatePreparePace::handleRetryCounter(int pRetryCounter)
{
	const auto& context = getContext();

	switch (pRetryCounter)
	{
		case 0:
		{
			qCDebug(statemachine) << "PUK required";
			context->setEstablishPaceChannelType(PacePasswordId::PACE_PUK);

			if (context->getPuk().isEmpty())
			{
				Q_EMIT fireEnterPacePassword();
				return;
			}

			Q_EMIT fireEstablishPaceChannel();
			return;
		}

		case 1:
		{
			qCDebug(statemachine) << "CAN required";
			if (requestPaceCanIfStillRequired())
			{
				return;
			}
		}
			Q_FALLTHROUGH();

		default:
		{
			qCDebug(statemachine) << "PIN allowed";
			context->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);

			const bool pacePinDone = context->getCardConnection()->getPacePinSuccessful();
			qCDebug(statemachine) << "PACE_PIN done:" << pacePinDone;
			if (!pacePinDone)
			{
				if (context->getPin().isEmpty())
				{
					Q_EMIT fireEnterPacePassword();
					return;
				}

				Q_EMIT fireEstablishPaceChannel();
				return;
			}

			Q_ASSERT(false && "This state must not be invoked any more if PACE_PIN already was successful.");
			Q_EMIT fireContinue();
			return;
		}
	}
}


void StatePreparePace::run()
{
	const auto& context = getContext();
	context->setEstablishPaceChannelType(PacePasswordId::UNKNOWN);
	const auto& cardConnection = context->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "Card connection lost.";
		Q_EMIT fireNoCardConnection();
		return;
	}

	const int currentRetryCounter = cardConnection->getReaderInfo().getRetryCounter();
	if (context->isSmartCardUsed())
	{
		if (currentRetryCounter == 0)
		{
			qCDebug(statemachine) << "Smart-eID was invalidated during workflow";
			updateStatus(GlobalStatus::Code::Card_Smart_Invalid);
			Q_EMIT fireAbort(FailureCode::Reason::Prepace_Pace_Smart_Eid_Invalidated);
			return;
		}

		qCDebug(statemachine) << "Smart-eID PIN required";
		context->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);
		if (context->getPin().isEmpty())
		{
			Q_EMIT fireEnterPacePassword();
			return;
		}

		Q_EMIT fireEstablishPaceChannel();
		return;
	}

	if (context->isCanAllowedMode())
	{
		qCDebug(statemachine) << "CAN allowed required";
		if (!requestPaceCanIfStillRequired())
		{
			Q_ASSERT(false && "This state must not be invoked any more if PACE_CAN was successful in CAN allowed mode.");
			Q_EMIT fireContinue();
		}
		return;
	}

	Q_ASSERT(currentRetryCounter != -1);
	handleRetryCounter(currentRetryCounter);
}


bool StatePreparePace::requestPaceCanIfStillRequired()
{
	getContext()->setEstablishPaceChannelType(PacePasswordId::PACE_CAN);

	const QSharedPointer<CardConnection>& cardConnection = getContext()->getCardConnection();
	const bool paceCanDone = cardConnection->getPaceCanSuccessful();
	qCDebug(statemachine) << "PACE_CAN done:" << paceCanDone;
	if (!paceCanDone)
	{
		if (getContext()->getCan().isEmpty())
		{
			Q_EMIT fireEnterPacePassword();
			return true;
		}

		Q_EMIT fireEstablishPaceChannel();
		return true;
	}

	return false;
}
