/*!
 *
 * \brief This state waits for editing of the effective CHAT
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractGenericState.h"
#include "context/AuthContext.h"

namespace governikus
{

class StateEditAccessRights
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} /* namespace governikus */
