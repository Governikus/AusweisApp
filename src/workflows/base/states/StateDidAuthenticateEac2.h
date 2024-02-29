/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "command/DidAuthenticateEAC2Command.h"
#include "context/AuthContext.h"

class test_StateDidAuthenticateEac2;

namespace governikus
{

class StateDidAuthenticateEac2
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateDidAuthenticateEac2;

	private:
		explicit StateDidAuthenticateEac2(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand);

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
