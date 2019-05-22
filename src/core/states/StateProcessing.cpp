/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateProcessing.h"

using namespace governikus;

StateProcessing::StateProcessing(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
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
