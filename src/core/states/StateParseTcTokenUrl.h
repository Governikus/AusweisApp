/*!
 * \brief Implements the parsing of TcTokenUrl in header of browser request.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"


namespace governikus
{

class StateParseTcTokenUrl
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateParseTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

};

} // namespace governikus
