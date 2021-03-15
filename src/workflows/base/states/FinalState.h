/*!
 * \brief A final state which blocks the state machine before termination.
 *
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
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
			AbstractState::onEntry(pEvent);
			getContext()->setWorkflowFinished(true);
		}

	public:
		explicit FinalState(const QSharedPointer<WorkflowContext>& pContext)
			: AbstractState(pContext, false)
			, GenericContextContainer(pContext)
		{
		}


};

} // namespace governikus
