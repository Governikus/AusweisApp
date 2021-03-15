/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/ChangePinContext.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StateEnterNewPacePin
	: public AbstractState
	, public GenericContextContainer<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateEnterNewPacePin(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
