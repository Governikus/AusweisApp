/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"

class test_StateUpdateRetryCounter;

namespace governikus
{

class StateUpdateRetryCounter
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateUpdateRetryCounter;

	private:
		explicit StateUpdateRetryCounter(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onUpdateRetryCounterDone(QSharedPointer<BaseCardCommand> pCommand);

	Q_SIGNALS:
		void fireNoCardConnection();
};

} // namespace governikus
