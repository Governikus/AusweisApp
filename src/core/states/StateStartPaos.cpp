/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "paos/invoke/StartPaos.h"
#include "StateStartPaos.h"

using namespace governikus;

StateStartPaos::StateStartPaos(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateStartPaos::run()
{
	Q_ASSERT(getContext()->getTcToken());

	auto sessionId = getContext()->getTcToken()->getSessionIdentifier();
	getContext()->setStartPaos(QSharedPointer<StartPaos>::create(sessionId));

	Q_EMIT fireContinue();
}
