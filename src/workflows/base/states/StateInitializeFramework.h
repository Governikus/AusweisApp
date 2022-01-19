/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"

#include <QSharedPointer>

namespace governikus
{

class StateInitializeFramework
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateInitializeFramework(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;
};

} // namespace governikus
