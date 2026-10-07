/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#include "StateClearPacePasswords.h"


using namespace governikus;


StateClearPacePasswords::StateClearPacePasswords(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateClearPacePasswords::run()
{
	getContext()->resetPacePasswords();

	Q_EMIT fireContinue();
}
