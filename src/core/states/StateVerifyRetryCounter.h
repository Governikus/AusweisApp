/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateVerifyRetryCounter
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateVerifyRetryCounter(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	Q_SIGNALS:
		void fireEnterPacePassword();
		void fireEstablishPaceChannel();
};

} // namespace governikus
