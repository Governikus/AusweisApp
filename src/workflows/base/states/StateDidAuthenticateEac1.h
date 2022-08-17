/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "command/DidAuthenticateEAC1Command.h"
#include "context/AuthContext.h"

class test_StateDidAuthenticateEac1;

namespace governikus
{

class StateDidAuthenticateEac1
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateDidAuthenticateEac1;

	private:
		explicit StateDidAuthenticateEac1(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand);

	public:
		void onEntry(QEvent* pEvent) override;

};

} // namespace governikus
