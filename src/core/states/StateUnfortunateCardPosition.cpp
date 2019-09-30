/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "StateUnfortunateCardPosition.h"

using namespace governikus;


StateUnfortunateCardPosition::StateUnfortunateCardPosition(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateUnfortunateCardPosition::run()
{
	getContext()->setNpaPositionVerified();
	Q_EMIT fireContinue();
}
