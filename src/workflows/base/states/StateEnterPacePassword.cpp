/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterPacePassword.h"


using namespace governikus;


StateEnterPacePassword::StateEnterPacePassword(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateEnterPacePassword::run()
{
	if (getContext()->getStatus().isError())
	{
		Q_EMIT fireAbort();
	}
	else
	{
		Q_EMIT fireContinue();
	}
}


void StateEnterPacePassword::onEntry(QEvent* pEvent)
{
	stopScanIfNecessary();

	AbstractState::onEntry(pEvent);
}
