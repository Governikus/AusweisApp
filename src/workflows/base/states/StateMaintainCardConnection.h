/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/WorkflowContext.h"

namespace governikus
{

class StateMaintainCardConnection
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		void handleWrongPacePassword();

	public:
		explicit StateMaintainCardConnection(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	Q_SIGNALS:
		void fireNoCardConnection();
		void fireForceUpdateRetryCounter();
		void firePropagateAbort();
};

} // namespace governikus
