/*
 * \brief Activates store the feedback dialog after a successful
 *        authentication.
 *
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"


namespace governikus
{

class StateActivateStoreFeedbackDialog
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateActivateStoreFeedbackDialog(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;
};

} // namespace governikus
