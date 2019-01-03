/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/ChangePinContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateEnterNewPacePin
	: public AbstractGenericState<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateEnterNewPacePin(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
