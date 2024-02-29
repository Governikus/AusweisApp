/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief This state executes the remote message PACE channel establish.
 */

#pragma once


#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


class test_StateEstablishPaceChannelResponse;


namespace governikus
{

class StateEstablishPaceChannelResponse
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateEstablishPaceChannelResponse;

	private:
		explicit StateEstablishPaceChannelResponse(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	Q_SIGNALS:
		void fireWrongPacePassword();
};

} // namespace governikus
