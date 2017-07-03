/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "paos/invoke/InitializeFrameworkResponse.h"
#include "states/StateInitializeFramework.h"

using namespace governikus;

StateInitializeFramework::StateInitializeFramework(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateInitializeFramework::run()
{
	Q_ASSERT(!getContext()->getInitializeFramework().isNull());
	Q_EMIT fireSuccess();
}
