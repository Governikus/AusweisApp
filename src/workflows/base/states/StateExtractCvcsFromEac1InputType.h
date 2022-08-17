/*!
 * \brief State to process InputType of EAC1. Extract certificates and set it to context.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"


class test_StateExtractCvcsFromEac1InputType;


namespace governikus
{

class StateExtractCvcsFromEac1InputType
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateExtractCvcsFromEac1InputType;

	private:
		explicit StateExtractCvcsFromEac1InputType(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
