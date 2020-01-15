/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/ChangePinContext.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StateEnterNewPacePin
	: public AbstractState
	, public GenericContextContainer<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateEnterNewPacePin(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
