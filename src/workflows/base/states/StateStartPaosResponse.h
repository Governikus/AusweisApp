/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief State to process StartPaosResponse from Server.
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"


class test_StateStartPaosResponse;


namespace governikus
{

class StateStartPaosResponse
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateStartPaosResponse;

	private:
		explicit StateStartPaosResponse(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

};

} // namespace governikus
