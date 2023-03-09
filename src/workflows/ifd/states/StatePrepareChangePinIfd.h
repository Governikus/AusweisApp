/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


namespace governikus
{

class StatePrepareChangePinIfd
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StatePrepareChangePinIfd(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	Q_SIGNALS:
		void fireEnterNewPacePin();
};

} // namespace governikus
