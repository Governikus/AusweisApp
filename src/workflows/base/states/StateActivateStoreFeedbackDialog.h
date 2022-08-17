/*
 * \brief Activates store the feedback dialog after a successful
 *        authentication.
 *
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
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
