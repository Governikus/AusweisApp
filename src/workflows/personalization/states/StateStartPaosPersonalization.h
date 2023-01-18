/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

namespace governikus
{

class StateStartPaosPersonalization
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateStartPaosPersonalization(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
