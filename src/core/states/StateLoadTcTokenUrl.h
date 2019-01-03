/*!
 * \brief Loads the tcTokenUrl for triggering the self authentication from secure storage
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/SelfAuthContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateLoadTcTokenUrl
	: public AbstractGenericState<SelfAuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateLoadTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
