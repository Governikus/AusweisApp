/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/WorkflowContext.h"


class test_StateVerifyRetryCounter;


namespace governikus
{

class StateVerifyRetryCounter
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateVerifyRetryCounter;

	private:
		explicit StateVerifyRetryCounter(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	Q_SIGNALS:
		void fireNoCardConnection();
		void fireReaderOrCardChanged();
};

} // namespace governikus
