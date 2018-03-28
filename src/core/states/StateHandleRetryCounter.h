/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"

#include "context/ChangePinContext.h"

namespace governikus
{

class StateHandleRetryCounter
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateHandleRetryCounter(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	Q_SIGNALS:
		void fireRetryCounterIsZero();
		void fireRetryCounterIsOne();
		void fireRetryCounterIsGTOne();
};

} /* namespace governikus */
