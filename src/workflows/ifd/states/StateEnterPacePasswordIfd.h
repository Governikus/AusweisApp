/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


class test_StateEnterPacePasswordIfd;


namespace governikus
{

class StateEnterPacePasswordIfd
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateEnterPacePasswordIfd;

	private:
		explicit StateEnterPacePasswordIfd(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onUserError();

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
