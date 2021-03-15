/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StatePreparePace.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StatePreparePace::StatePreparePace(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StatePreparePace::run()
{
	getContext()->setEstablishPaceChannelType(PacePasswordId::UNKNOWN);
	const QSharedPointer<CardConnection>& cardConnection = getContext()->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "Card connection lost.";
		Q_EMIT fireAbort();
		return;
	}

	if (getContext()->isCanAllowedMode())
	{
		qCDebug(statemachine) << "CAN allowed required";
		if (!requestPaceCanIfStillRequired())
		{
			Q_ASSERT(false && "This state must not be invoked any more if PACE_CAN was successful in CAN allowed mode.");
			Q_EMIT fireContinue();
		}
		return;
	}

	const int currentRetryCounter = cardConnection->getReaderInfo().getRetryCounter();
	Q_ASSERT(currentRetryCounter != -1);
	switch (currentRetryCounter)
	{
		case 0:
		{
			qCDebug(statemachine) << "PUK required";
			getContext()->setEstablishPaceChannelType(PacePasswordId::PACE_PUK);

			if (getContext()->getPuk().isEmpty())
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
			getContext()->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);

			const bool pacePinDone = cardConnection->getPacePinSuccessful();
			qCDebug(statemachine) << "PACE_PIN done:" << pacePinDone;
			if (!pacePinDone)
			{
				if (getContext()->getPin().isEmpty())
				{
					Q_EMIT fireEnterPacePassword();
					return;
				}

				Q_EMIT fireEstablishPaceChannel();
				return;
			}

			Q_UNREACHABLE();
		}
	}

	Q_UNREACHABLE();
	Q_EMIT fireContinue();
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
