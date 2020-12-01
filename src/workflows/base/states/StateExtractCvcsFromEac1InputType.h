/*!
 * \brief State to process InputType of EAC1. Extract certificates and set it to context.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"


namespace governikus
{

class StateExtractCvcsFromEac1InputType
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateExtractCvcsFromEac1InputType(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
