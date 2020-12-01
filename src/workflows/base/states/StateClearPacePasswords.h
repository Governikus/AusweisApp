/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
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
	virtual void run() override;
};

} // namespace governikus
