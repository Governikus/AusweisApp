/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief State to process StartPaosResponse from Server.
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


class test_StateStartPaosPersonalizationResponse;

namespace governikus
{

class StateStartPaosPersonalizationResponse
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateStartPaosPersonalizationResponse;

	private:
		explicit StateStartPaosPersonalizationResponse(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

};

} // namespace governikus
