/*
 *
 * \brief Writes the history entry .
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"


class test_StateWriteHistory;


namespace governikus
{

class StateWriteHistory
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateWriteHistory;

	private:
		explicit StateWriteHistory(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
