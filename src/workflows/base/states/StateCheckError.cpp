/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
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
	if (getContext()->getStatus().isNoError() || getContext()->isErrorReportedToUser())
	{
		Q_EMIT fireContinue();
		return;
	}

	Q_EMIT fireAbort();
}
