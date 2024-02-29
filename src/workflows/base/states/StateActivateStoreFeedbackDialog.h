/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Activates store the feedback dialog after a successful
 *        authentication.
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"


namespace governikus
{

class StateActivateStoreFeedbackDialog
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateActivateStoreFeedbackDialog(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
