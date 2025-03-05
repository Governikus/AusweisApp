/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/WorkflowContext.h"


class test_StatePreparePace;


namespace governikus
{

class StatePreparePace
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StatePreparePace;

	private:
		explicit StatePreparePace(const QSharedPointer<WorkflowContext>& pContext);
		void handleRetryCounter(int pRetryCounter);
		void run() override;

		bool requestPaceCanIfStillRequired();

	Q_SIGNALS:
		void fireNoCardConnection();
		void fireEnterPacePassword();
};

} // namespace governikus
