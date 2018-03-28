/*!
 * \brief Controller for the step that updates the retry
 *        counter of a card.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"

#include "context/ChangePinContext.h"

namespace governikus
{

class StateUpdateRetryCounter
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateUpdateRetryCounter(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onUpdateRetryCounterDone(QSharedPointer<BaseCardCommand> pCommand);
};

} /* namespace governikus */
