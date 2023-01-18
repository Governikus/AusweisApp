/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateStartPaos.h"

#include "CardConnection.h"
#include "paos/invoke/StartPaos.h"

using namespace governikus;

StateStartPaos::StateStartPaos(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
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
