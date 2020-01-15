/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterNewPacePin.h"


using namespace governikus;


StateEnterNewPacePin::StateEnterNewPacePin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateEnterNewPacePin::run()
{
	Q_EMIT fireContinue();
}
