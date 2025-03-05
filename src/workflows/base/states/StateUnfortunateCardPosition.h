/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/WorkflowContext.h"

namespace governikus
{

class StateUnfortunateCardPosition
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateUnfortunateCardPosition(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
