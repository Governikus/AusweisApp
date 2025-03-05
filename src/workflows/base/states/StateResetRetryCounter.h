/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/WorkflowContext.h"

class test_StateResetRetryCounter;

namespace governikus
{

class StateResetRetryCounter
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateResetRetryCounter;

	private:
		explicit StateResetRetryCounter(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onResetRetryCounterDone(QSharedPointer<BaseCardCommand> pCommand);

	Q_SIGNALS:
		void fireNoCardConnection();

};

} // namespace governikus
