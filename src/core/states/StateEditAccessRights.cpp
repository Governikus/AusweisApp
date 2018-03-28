/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateEditAccessRights.h"

using namespace governikus;

StateEditAccessRights::StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateEditAccessRights::run()
{
	Q_EMIT fireContinue();
}
