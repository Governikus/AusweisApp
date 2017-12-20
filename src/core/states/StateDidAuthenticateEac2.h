/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "command/DidAuthenticateEAC2Command.h"
#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateDidAuthenticateEac2
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateDidAuthenticateEac2(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand);
};

} /* namespace governikus */
