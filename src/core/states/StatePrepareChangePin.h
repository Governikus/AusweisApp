/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/ChangePinContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StatePrepareChangePin
	: public AbstractGenericState<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StatePrepareChangePin(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	Q_SIGNALS:
		void fireEnterNewPacePin();
};

} // namespace governikus
