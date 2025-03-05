/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/WorkflowContext.h"

class test_StateEnterPacePassword;

namespace governikus
{

class StateEnterPacePassword
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateEnterPacePassword;

	private:
		explicit StateEnterPacePassword(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
