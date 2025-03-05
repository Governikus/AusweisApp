/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

namespace governikus
{

class StateLoadSmartTcTokenUrl
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateLoadSmartTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
