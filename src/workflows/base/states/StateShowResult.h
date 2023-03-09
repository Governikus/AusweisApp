/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"


namespace governikus
{
class StateShowResult
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateShowResult(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
