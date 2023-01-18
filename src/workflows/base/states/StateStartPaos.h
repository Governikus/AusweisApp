/*!
 * \brief State machine state: Start PAOS.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"

namespace governikus
{

class StateStartPaos
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateStartPaos(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
