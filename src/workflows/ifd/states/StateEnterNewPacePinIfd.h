/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

namespace governikus
{

class StateEnterNewPacePinIfd
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateEnterNewPacePinIfd(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onUserError(StatusCode pStatusCode);
		void onCardRemoved(const ReaderInfo& pInfo) const;

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
