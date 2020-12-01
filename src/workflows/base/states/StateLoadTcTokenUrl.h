/*!
 * \brief Loads the tcTokenUrl for triggering the self authentication from secure storage
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/SelfAuthContext.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StateLoadTcTokenUrl
	: public AbstractState
	, public GenericContextContainer<SelfAuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateLoadTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
