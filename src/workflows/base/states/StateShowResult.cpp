/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateShowResult.h"

using namespace governikus;


StateShowResult::StateShowResult(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateShowResult::run()
{
	Q_EMIT fireContinue();
}
