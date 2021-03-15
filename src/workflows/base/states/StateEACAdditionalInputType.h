/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StateEACAdditionalInputType
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateEACAdditionalInputType(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;

	Q_SIGNALS:
		void fireSendDidAuthenticatResponse();
};

} // namespace governikus
