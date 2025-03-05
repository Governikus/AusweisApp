/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/ChangePinContext.h"

class test_StatePrepareChangePin;

namespace governikus
{

class StatePrepareChangePin
	: public AbstractState
	, public GenericContextContainer<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StatePrepareChangePin;

	private:
		explicit StatePrepareChangePin(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	Q_SIGNALS:
		void fireEnterNewPacePin();
		void fireSkipPinChange();
};

} // namespace governikus
