/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateProcessing.h"

using namespace governikus;

StateProcessing::StateProcessing(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateProcessing::run()
{
	auto activationContext = getContext()->getActivationContext();
	if (activationContext->sendProcessing())
	{
		Q_EMIT fireContinue();
	}
	else
	{
		qCritical() << "Cannot send \"Processing\" to caller:" << activationContext->getSendError();
		updateStatus(GlobalStatus(GlobalStatus::Code::Workflow_Processing_Error, activationContext->getSendError()));
		Q_EMIT fireAbort();
	}
}
