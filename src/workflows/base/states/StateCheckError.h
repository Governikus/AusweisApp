/*!
 * \brief Helper state to decide whether an error has been occurred that should
 * be reported to the user.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"

namespace governikus
{

class StateCheckError
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateCheckError(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
