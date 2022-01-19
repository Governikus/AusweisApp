/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/WorkflowContext.h"
#include "GenericContextContainer.h"


namespace governikus
{

class StatePreparePace
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StatePreparePace(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;

	bool requestPaceCanIfStillRequired();

	Q_SIGNALS:
		void fireEnterPacePassword();
		void fireEstablishPaceChannel();
};

} // namespace governikus
