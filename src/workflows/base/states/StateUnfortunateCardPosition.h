/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/WorkflowContext.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StateUnfortunateCardPosition
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateUnfortunateCardPosition(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
