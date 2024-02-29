/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief A final state which blocks the state machine before termination.
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"

namespace governikus
{

class FinalState
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT

	private:
		void run() override;

	protected:
		void onEntry(QEvent* pEvent) override
		{
			getContext()->setWorkflowFinished(true);
			AbstractState::onEntry(pEvent);
		}

	public:
		explicit FinalState(const QSharedPointer<WorkflowContext>& pContext)
			: AbstractState(pContext)
			, GenericContextContainer(pContext)
		{
		}


};

} // namespace governikus
