/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateEditAccessRights.h"

using namespace governikus;

StateEditAccessRights::StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateEditAccessRights::run()
{
	Q_EMIT fireContinue();
}
