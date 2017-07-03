/*!
 * \brief Loads the tcTokenUrl for triggering the self authentication from secure storage
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/SelfAuthenticationContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateLoadTcTokenUrl
	: public AbstractGenericState<SelfAuthenticationContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateLoadTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} /* namespace governikus */
