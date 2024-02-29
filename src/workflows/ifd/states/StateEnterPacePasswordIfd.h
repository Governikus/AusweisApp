/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

namespace governikus
{

class StateEnterPacePasswordIfd
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateEnterPacePasswordIfd(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onCancelEstablishPaceChannel();

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
