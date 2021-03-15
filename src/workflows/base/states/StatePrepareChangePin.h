/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/ChangePinContext.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StatePrepareChangePin
	: public AbstractState
	, public GenericContextContainer<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StatePrepareChangePin(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;

	Q_SIGNALS:
		void fireEnterNewPacePin();
};

} // namespace governikus
