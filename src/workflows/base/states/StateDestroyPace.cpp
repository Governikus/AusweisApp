/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */


#include "StateDestroyPace.h"


Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StateDestroyPace::StateDestroyPace(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateDestroyPace::run()
{
	auto cardConnection = getContext()->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "No card connection available.";
		Q_EMIT fireAbort(FailureCode::Reason::Destroy_Pace_No_Connection_To_Destroy);
		return;
	}

	mConnections += cardConnection->callDestroyPaceChannelCommand(this, &StateDestroyPace::onDestroyPaceDone);
}


void StateDestroyPace::onDestroyPaceDone(QSharedPointer<BaseCardCommand>)
{
	Q_EMIT fireContinue();
}
