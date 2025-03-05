/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/ChangePinContext.h"

namespace governikus
{

class StateEnterNewPacePin
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateEnterNewPacePin(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
