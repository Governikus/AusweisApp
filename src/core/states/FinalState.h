/*!
 * FinalState.h
 *
 * \brief A final state which blocks the state machine before termination.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "states/AbstractGenericState.h"


namespace governikus
{

class FinalState
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		virtual void run() override;

	protected:
		void onEntry(QEvent* pEvent) override
		{
			AbstractGenericState::onEntry(pEvent);
			getContext()->setWorkflowFinished(true);
		}


	public:
		FinalState(const QSharedPointer<WorkflowContext>& pContext)
			: AbstractGenericState(pContext, false)
		{
		}


};

} /* namespace governikus */
