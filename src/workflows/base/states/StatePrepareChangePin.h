/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/ChangePinContext.h"

namespace governikus
{

class StatePrepareChangePin
	: public AbstractState
	, public GenericContextContainer<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StatePrepareChangePin(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	Q_SIGNALS:
		void fireEnterNewPacePin();
};

} // namespace governikus
