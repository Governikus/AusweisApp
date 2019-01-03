/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateMaintainCardConnection
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	public:
		explicit StateMaintainCardConnection(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	Q_SIGNALS:
		void fireNoCardConnection();
		void fireForceUpdateRetryCounter();
};

} // namespace governikus
