/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implements the parsing of TcTokenUrl in header of browser request.
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"


namespace governikus
{

class StateParseTcTokenUrl
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateParseTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

};

} // namespace governikus
