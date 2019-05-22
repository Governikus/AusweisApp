/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterNewPacePin.h"


using namespace governikus;


StateEnterNewPacePin::StateEnterNewPacePin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateEnterNewPacePin::run()
{
	Q_EMIT fireContinue();
}
