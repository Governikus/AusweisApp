/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"

class test_StateDestroyPace;

namespace governikus
{

class StateDestroyPace
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateDestroyPace;

	private:
		explicit StateDestroyPace(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onDestroyPaceDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
