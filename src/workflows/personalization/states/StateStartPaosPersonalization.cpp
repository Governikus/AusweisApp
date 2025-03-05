/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateStartPaosPersonalization.h"

#include "CardConnection.h"
#include "paos/invoke/StartPaos.h"

using namespace governikus;

StateStartPaosPersonalization::StateStartPaosPersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateStartPaosPersonalization::run()
{
	const auto& context = getContext();
	const auto& sessionId = context->getSessionIdentifier();
	context->setStartPaos(QSharedPointer<StartPaos>::create(sessionId.toByteArray(QUuid::WithoutBraces)));

	Q_EMIT fireContinue();
}
