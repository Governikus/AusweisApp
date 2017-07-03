/*!
 * \brief State to process StartPaosResponse from Server.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateStartPaosResponse
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateStartPaosResponse(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

};

} /* namespace governikus */
