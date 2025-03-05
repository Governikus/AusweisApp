/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateCheckError.h"

using namespace governikus;

StateCheckError::StateCheckError(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateCheckError::run()
{
	if (getContext()->getStatus().isNoError())
	{
		Q_EMIT fireContinue();
		return;
	}

	Q_ASSERT(getContext()->getFailureCode().has_value());
	Q_EMIT firePropagateAbort();
}
