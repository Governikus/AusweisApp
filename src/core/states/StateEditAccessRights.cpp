/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateEditAccessRights.h"

using namespace governikus;

StateEditAccessRights::StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateEditAccessRights::run()
{
	Q_EMIT fireContinue();
}
