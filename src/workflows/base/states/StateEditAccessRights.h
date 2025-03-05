/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"

namespace governikus
{

class StateEditAccessRights
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
