/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StateCleanUpReaderManager
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateCleanUpReaderManager(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
