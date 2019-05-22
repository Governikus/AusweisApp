/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateUnfortunateCardPosition.h"


using namespace governikus;


StateUnfortunateCardPosition::StateUnfortunateCardPosition(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateUnfortunateCardPosition::run()
{
	getContext()->setNpaPositionVerified();
	Q_EMIT fireContinue();
}
