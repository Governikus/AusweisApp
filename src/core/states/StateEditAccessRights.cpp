/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "StateEditAccessRights.h"

using namespace governikus;

StateEditAccessRights::StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateEditAccessRights::run()
{
	Q_EMIT fireSuccess();
}
