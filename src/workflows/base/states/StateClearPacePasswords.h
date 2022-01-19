/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/WorkflowContext.h"
#include "GenericContextContainer.h"


namespace governikus
{

class StateClearPacePasswords
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateClearPacePasswords(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;
};

} // namespace governikus
