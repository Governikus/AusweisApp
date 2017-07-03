/*!
 * StateCheckError.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StateCheckError.h"

using namespace governikus;

StateCheckError::StateCheckError(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateCheckError::run()
{
	if (getContext()->getResult().isOk() || getContext()->isErrorReportedToUser())
	{
		Q_EMIT fireSuccess();
		return;
	}

	Q_EMIT fireError();
}
