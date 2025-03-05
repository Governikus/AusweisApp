/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StatePrepareChangePin.h"

#include <QDebug>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(statemachine)


StatePrepareChangePin::StatePrepareChangePin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StatePrepareChangePin::run()
{
	if (getContext()->isOnlyCheckPin())
	{
		Q_EMIT fireSkipPinChange();
		return;
	}

	if (getContext()->getNewPin().isEmpty())
	{
		Q_EMIT fireEnterNewPacePin();
		return;
	}

	Q_EMIT fireContinue();
}
