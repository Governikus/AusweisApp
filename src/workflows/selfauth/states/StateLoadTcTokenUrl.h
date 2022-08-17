/*!
 * \brief Loads the tcTokenUrl for triggering the self authentication from secure storage
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/SelfAuthContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

namespace governikus
{

class StateLoadTcTokenUrl
	: public AbstractState
	, public GenericContextContainer<SelfAuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateLoadTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
