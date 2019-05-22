/*!
 *
 * \brief This state waits for editing of the effective CHAT
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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

	explicit StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
