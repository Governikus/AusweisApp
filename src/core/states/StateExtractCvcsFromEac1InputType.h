/*!
 * \brief State to process InputType of EAC1. Extract certificates and set it to context.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

class test_StateExtractCvcsFromEac1InputType;

namespace governikus
{

class StateExtractCvcsFromEac1InputType
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateExtractCvcsFromEac1InputType;

	StateExtractCvcsFromEac1InputType(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} /* namespace governikus */
