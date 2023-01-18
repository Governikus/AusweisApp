/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
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

	public:
		explicit StateMaintainCardConnection(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	Q_SIGNALS:
		void fireNoCardConnection();
		void fireForceUpdateRetryCounter();
};

} // namespace governikus
