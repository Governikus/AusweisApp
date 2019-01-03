/*!
 * \brief Process received transmits. Send it to the card and create a response.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "command/TransmitCommand.h"
#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

class test_StateTransmit;

namespace governikus
{

class StateTransmit
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateTransmit(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		friend class ::test_StateTransmit;
		void onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
