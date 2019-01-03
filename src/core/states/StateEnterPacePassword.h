/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateEnterPacePassword
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateEnterPacePassword(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
