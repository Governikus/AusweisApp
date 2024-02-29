/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief This state executes the remote message IfdModifyPin.
 */

#pragma once


#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

namespace governikus
{

class StateChangePinResponse
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateChangePinResponse(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	Q_SIGNALS:
		void firePacePasswordModified();
};

} // namespace governikus
