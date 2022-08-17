/*!
 * \brief State that checks if the Smart-eID functionality is given
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

namespace governikus
{

class StateCheckStatus
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateCheckStatus(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
