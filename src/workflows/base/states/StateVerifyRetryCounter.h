/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/WorkflowContext.h"
#include "GenericContextContainer.h"


namespace governikus
{

class StateVerifyRetryCounter
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateVerifyRetryCounter(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;

	Q_SIGNALS:
		void fireEnterPacePassword();
		void fireEstablishPaceChannel();
};

} // namespace governikus
