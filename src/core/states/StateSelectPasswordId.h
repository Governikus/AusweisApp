/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"

#include "context/WorkflowContext.h"

namespace governikus
{

class StateSelectPasswordId
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateSelectPasswordId(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	Q_SIGNALS:
		void firePasswordIdCAN();
};

} /* namespace governikus */
