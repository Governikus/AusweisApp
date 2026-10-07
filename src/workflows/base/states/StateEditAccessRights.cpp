/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
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
