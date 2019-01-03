/*
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateShowSelfInfo.h"

using namespace governikus;


StateShowSelfInfo::StateShowSelfInfo(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateShowSelfInfo::run()
{
	Q_EMIT fireContinue();
}
