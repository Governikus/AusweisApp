/*!
 * \brief Process received transmits. Send it to the card and create a response.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "command/TransmitCommand.h"
#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateTransmit
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateTransmit(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand);
};

} /* namespace governikus */
