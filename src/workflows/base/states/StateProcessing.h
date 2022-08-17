/*!
 * \brief Sends a HTTP-Processing to the browser.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"

namespace governikus
{

class StateProcessing
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateProcessing(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
