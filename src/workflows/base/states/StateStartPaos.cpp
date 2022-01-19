/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "paos/invoke/StartPaos.h"
#include "StateStartPaos.h"

using namespace governikus;

StateStartPaos::StateStartPaos(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateStartPaos::run()
{
	Q_ASSERT(getContext()->getTcToken());

	auto sessionId = getContext()->getTcToken()->getSessionIdentifier();
	getContext()->setStartPaos(QSharedPointer<StartPaos>::create(sessionId));

	Q_EMIT fireContinue();
}
