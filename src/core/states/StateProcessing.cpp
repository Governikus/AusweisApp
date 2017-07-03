/*!
 * StateProcessing.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
		Q_EMIT fireSuccess();
	}
	else
	{
		qCritical() << "Cannot send \"Processing\" to caller: " << activationContext->getSendError();
		setResult(Result::createCommunicationError(activationContext->getSendError()));
		Q_EMIT fireError();
	}
}
