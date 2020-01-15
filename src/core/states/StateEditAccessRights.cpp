/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "StateEditAccessRights.h"

using namespace governikus;

StateEditAccessRights::StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateEditAccessRights::run()
{
	Q_EMIT fireContinue();
}
