/*!
 * \brief Helper state to decide whether an error has been occurred that should
 * be reported to the user.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateCheckError
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateCheckError(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} /* namespace governikus */
