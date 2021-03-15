/*!
 * \brief Sends a HTTP-Processing to the browser.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StateProcessing
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateProcessing(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;
};

} // namespace governikus
