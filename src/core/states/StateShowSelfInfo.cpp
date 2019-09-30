/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateShowSelfInfo.h"

using namespace governikus;


StateShowSelfInfo::StateShowSelfInfo(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateShowSelfInfo::run()
{
	Q_EMIT fireContinue();
}
