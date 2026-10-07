/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/WorkflowContext.h"


namespace governikus
{

class StateClearPacePasswords
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateClearPacePasswords(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
