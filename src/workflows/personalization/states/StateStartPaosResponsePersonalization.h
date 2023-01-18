/*!
 * \brief State to process StartPaosResponse from Server.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

namespace governikus
{

class StateStartPaosResponsePersonalization
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateStartPaosResponsePersonalization(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

};

} // namespace governikus
