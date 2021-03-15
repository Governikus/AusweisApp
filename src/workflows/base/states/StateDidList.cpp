/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "paos/invoke/DidListResponse.h"
#include "StateDidList.h"


using namespace governikus;


StateDidList::StateDidList(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateDidList::run()
{
	Q_ASSERT(!getContext()->getDidList().isNull());
	Q_EMIT fireContinue();
}
